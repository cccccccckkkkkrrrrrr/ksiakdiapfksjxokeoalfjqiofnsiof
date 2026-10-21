package multi.decompiler.by.pbcat;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.Typeface;
import android.graphics.drawable.GradientDrawable;
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
import java.io.InputStream;
import java.io.FileOutputStream;

public class Jadx extends Activity {
    private static final int PICK_FILE_CODE = 100;
    private String selectedPath = "";
    private String fileInfo = "";
    private String mode = "apk";
    private TextView infoView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mode = getIntent().getStringExtra("mode");
        if (mode == null) mode = "apk";

        requestStoragePermissions();

        ScrollView scroll = new ScrollView(this);
        scroll.setBackgroundColor(Color.BLACK);
        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setGravity(Gravity.CENTER_HORIZONTAL);
        layout.setPadding(40, 80, 40, 100);

        TextView title = new TextView(this);
        title.setText(mode.equals("apk") ? "Decompile APK" : "Decompile DEX");
        title.setTextColor(Color.WHITE);
        title.setTextSize(24);
        title.setTypeface(null, Typeface.BOLD);
        title.setPadding(0, 0, 0, 40);
        layout.addView(title);

        Button pickBtn = new Button(this);
        pickBtn.setText("SELECT FILE");
        GradientDrawable gd = new GradientDrawable(GradientDrawable.Orientation.LEFT_RIGHT, new int[]{Color.parseColor("#FF0000"), Color.parseColor("#FF8C00")});
        gd.setCornerRadius(50);
        gd.setStroke(3, Color.parseColor("#FFFF00"));
        pickBtn.setBackground(new android.graphics.drawable.RippleDrawable(android.content.res.ColorStateList.valueOf(Color.parseColor("#44FFFFFF")), gd, null));
        pickBtn.setTextColor(Color.WHITE);
        pickBtn.setTypeface(null, Typeface.BOLD);
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

        infoView = new TextView(this);
        infoView.setTextColor(Color.WHITE);
        infoView.setTextSize(14);
        infoView.setPadding(20, 20, 20, 20);
        infoView.setBackgroundColor(Color.parseColor("#1A1A1A"));
        infoView.setVisibility(View.GONE);
        LinearLayout.LayoutParams infoLp = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        infoLp.topMargin = 20;
        infoLp.bottomMargin = 20;
        layout.addView(infoView, infoLp);

        Button decompileBtn = new Button(this);
        decompileBtn.setText("DECOMPILE TO JAVA");
        GradientDrawable gd2 = new GradientDrawable(GradientDrawable.Orientation.LEFT_RIGHT, new int[]{Color.parseColor("#00FFFF"), Color.parseColor("#0000FF")});
        gd2.setCornerRadius(50);
        gd2.setStroke(3, Color.parseColor("#FFFFFF"));
        decompileBtn.setBackground(new android.graphics.drawable.RippleDrawable(android.content.res.ColorStateList.valueOf(Color.parseColor("#44FFFFFF")), gd2, null));
        decompileBtn.setTextColor(Color.WHITE);
        decompileBtn.setTypeface(null, Typeface.BOLD);
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

                            // Пытаемся отключить защиту через системное свойство Java
                            System.setProperty("JADX_DISABLE_ZIP_SECURITY", "true");
                            // Дополнительный флаг на случай, если JADX читает его как env
                            try {
                                java.lang.reflect.Field env = System.getenv().getClass().getDeclaredField("m");
                                env.setAccessible(true);
                                java.util.Map<String,String> map = (java.util.Map<String,String>) env.get(System.getenv());
                                map.put("JADX_DISABLE_ZIP_SECURITY", "true");
                            } catch (Exception ignored) {}

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
                                    // Ловим именно ошибку ZipSecurity и показываем понятный текст
                                    if (err != null && err.contains("JadxZipSecurity")) {
                                        Toast.makeText(Jadx.this, "Файл защищён (ZipSecurity). JADX не может его открыть.", Toast.LENGTH_LONG).show();
                                    } else {
                                        Toast.makeText(Jadx.this, "Error: " + err, Toast.LENGTH_LONG).show();
                                    }
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
                    InputStream is = getContentResolver().openInputStream(uri);
                    File tempFile = new File(getCacheDir(), "input_file.bin");
                    FileOutputStream os = new FileOutputStream(tempFile);
                    byte[] buffer = new byte[4096];
                    int len;
                    while ((len = is.read(buffer)) > 0) os.write(buffer, 0, len);
                    os.close();
                    is.close();
                    selectedPath = tempFile.getAbsolutePath();

                    android.content.pm.PackageInfo pi = getPackageManager().getPackageArchiveInfo(selectedPath, 0);
                    if (pi != null) {
                        fileInfo = "Package: " + pi.packageName + "\nVersion: " + pi.versionName + "\nVersion Code: " + pi.versionCode;
                        android.content.pm.ApplicationInfo ai = pi.applicationInfo;
                        if (ai != null) {
                            ai.sourceDir = selectedPath;
                            ai.publicSourceDir = selectedPath;
                            fileInfo = "App: " + getPackageManager().getApplicationLabel(ai) + "\n" + fileInfo;
                        }
                    } else {
                        fileInfo = "File: " + tempFile.getName() + "\nSize: " + tempFile.length() + " bytes";
                    }

                    infoView.setText(fileInfo);
                    infoView.setVisibility(View.VISIBLE);
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
