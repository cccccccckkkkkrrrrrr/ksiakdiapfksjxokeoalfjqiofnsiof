package multi.decompiler.by.pbcat;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Toast;
import java.io.File;

public class Jadx extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setPadding(40, 40, 40, 40);
        
        final EditText apkPath = new EditText(this);
        apkPath.setHint("Path to APK (e.g. /storage/emulated/0/app.apk)");
        layout.addView(apkPath);
        
        Button decompileBtn = new Button(this);
        decompileBtn.setText("Decompile to Java");
        decompileBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String path = apkPath.getText().toString();
                if (path.isEmpty()) {
                    Toast.makeText(Jadx.this, "Enter APK path", Toast.LENGTH_SHORT).show();
                    return;
                }
                
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            File apk = new File(apkPath.getText().toString());
                            if (!apk.exists()) {
                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        Toast.makeText(Jadx.this, "File not found", Toast.LENGTH_SHORT).show();
                                    }
                                });
                                return;
                            }
                            
                            File outDir = new File(Environment.getExternalStorageDirectory(), "Decompiled_Jadx");
                            if (!outDir.exists()) outDir.mkdirs();
                            
                            jadx.api.JadxArgs args = new jadx.api.JadxArgs();
                            args.setInputFile(apk);
                            args.setOutDir(outDir);
                            
                            jadx.api.JadxDecompiler decompiler = new jadx.api.JadxDecompiler(args);
                            decompiler.load();
                            decompiler.save();
                            decompiler.close();
                            
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    Toast.makeText(Jadx.this, "Decompiled! Check " + outDir.getAbsolutePath(), Toast.LENGTH_LONG).show();
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
        layout.addView(decompileBtn);
        setContentView(layout);
    }
}
