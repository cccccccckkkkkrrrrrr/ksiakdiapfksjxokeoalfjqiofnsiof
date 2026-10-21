package multi.decompiler.by.pbcat;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;
import java.io.File;

public class Jadx extends Activity {
    private static final int PICK_FILE_CODE = 100;
    private String selectedPath = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestStoragePermissions();

        ScrollView scroll = new ScrollView(this);
        scroll.setBackgroundColor(Color.BLACK);
        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setGravity(Gravity.CENTER_HORIZONTAL);
        layout.setPadding(40, 100, 40, 100);

        TextView title = new TextView(this);
        title.setText("JADX Decompiler");
        title.setTextColor(Color.WHITE);
        title.setTextSize(24);
        title.setTypeface(null, android.graphics.Typeface.BOLD);
        title.setPadding(0, 0, 0, 40);
        layout.addView(title);

        Button pickBtn = new Button(this);
        pickBtn.setText("1. SELECT APK / DEX FILE");
        layout.addView(pickBtn, new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, 140));
        pickBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
                intent.addCategory(Intent.CATEGORY_OPENABLE);
                intent.setType("*/*");
                startActivityForResult(intent, PICK_FILE_CODE);
            }
        });

        Button decompileBtn = new Button(this);
        decompileBtn.setText("2. DECOMPILE TO JAVA");
        layout.addView(decompileBtn, new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, 140));
        decompileBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (selectedPath.isEmpty()) {
                    Toast.makeText(Jadx.this, "Сначала выбери файл!", Toast.LENGTH_SHORT).show();
                    return;
                }
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            File input = new File(selectedPath);
                            File outDir = new File(Environment.getExternalStorageDirectory(), "Decompiled_Jadx");
                            if (!outDir.exists()) outDir.mkdirs();

                            // Отключаем проверку безопасности Zip (чтобы избежать ошибки JadxZipSecurity)
                            System.setProperty("JADX_DISABLE_ZIP_SECURITY", "true");

                            jadx.api.JadxArgs args = new jadx.api.JadxArgs();
                            args.setInputFile(input);
                            args.setOutDir(outDir);

                            jadx.api.JadxDecompiler decompiler = new jadx.api.JadxDecompiler(args);
                            decompiler.load();
                            decompiler.save();
                            decompiler.close();

                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    Toast.makeText(Jadx.this, "Готово! Смотри /storage/emulated/0/Decompiled_Jadx", Toast.LENGTH_LONG).show();
                                }
                            });
                        } catch (Throwable e) {
                            final String err = e.getMessage();
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    Toast.makeText(Jadx.this, "Error: " + err, Toast.LENGTH_LONG).show();
                                }
                            });
                        }
                    }
                }).start();
            }
        });

        scroll.addView(layout);
        setContentView(scroll);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == PICK_FILE_CODE && resultCode == RESULT_OK && data != null) {
            Uri uri = data.getData();
            if (uri != null) {
                try {
                    java.io.InputStream is = getContentResolver().openInputStream(uri);
                    File tempFile = new File(getCacheDir(), "input_file.bin");
                    java.io.FileOutputStream os = new java.io.FileOutputStream(tempFile);
                    byte[] buffer = new byte[4096];
                    int len;
                    while ((len = is.read(buffer)) > 0) os.write(buffer, 0, len);
                    os.close();
                    is.close();
                    selectedPath = tempFile.getAbsolutePath();
                    Toast.makeText(this, "Файл выбран", Toast.LENGTH_SHORT).show();
                } catch (Exception e) {
                    Toast.makeText(this, "Ошибка чтения файла", Toast.LENGTH_SHORT).show();
                }
            }
        }
    }

    private void requestStoragePermissions() {
        if (Build.VERSION.SDK_INT >= 33) {
            requestPermissions(new String[]{
                    android.Manifest.permission.READ_MEDIA_IMAGES,
                    android.Manifest.permission.READ_MEDIA_VIDEO,
                    android.Manifest.permission.READ_MEDIA_AUDIO
            }, 1);
        } else {
            requestPermissions(new String[]{
                    android.Manifest.permission.READ_EXTERNAL_STORAGE,
                    android.Manifest.permission.WRITE_EXTERNAL_STORAGE
            }, 1);
        }
    }
}
