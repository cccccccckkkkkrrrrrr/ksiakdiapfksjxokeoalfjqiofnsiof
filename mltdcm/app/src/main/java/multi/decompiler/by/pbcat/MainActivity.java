package multi.decompiler.by.pbcat;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.Typeface;
import android.graphics.drawable.GradientDrawable;
import android.graphics.drawable.RippleDrawable;
import android.content.res.ColorStateList;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.view.Gravity;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {

    private void applyMegaButtonStyle(Button btn, String startColor, String endColor, String strokeColor, String textColor) {
        GradientDrawable gd = new GradientDrawable(
                GradientDrawable.Orientation.LEFT_RIGHT,
                new int[]{Color.parseColor(startColor), Color.parseColor(endColor)}
        );
        gd.setCornerRadius(50);
        gd.setStroke(3, Color.parseColor(strokeColor));
        RippleDrawable ripple = new RippleDrawable(ColorStateList.valueOf(Color.parseColor("#44FFFFFF")), gd, null);
        btn.setBackground(ripple);
        btn.setTextColor(Color.parseColor(textColor));
        btn.setTextSize(16);
        btn.setTypeface(null, Typeface.BOLD);
        btn.setAllCaps(false);
        if (Build.VERSION.SDK_INT >= 21) btn.setElevation(20);
        btn.setShadowLayer(15, 0, 0, Color.parseColor(strokeColor));
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (Build.VERSION.SDK_INT >= 30) {
            if (!Environment.isExternalStorageManager()) {
                Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                intent.setData(Uri.parse("package:" + getPackageName()));
                startActivity(intent);
            }
        }

        Window window = getWindow();
        if (Build.VERSION.SDK_INT >= 21) {
            window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
            window.setStatusBarColor(Color.TRANSPARENT);
            window.setNavigationBarColor(Color.BLACK);
            window.getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN);
        }

        ScrollView scroll = new ScrollView(this);
        scroll.setBackgroundColor(Color.BLACK);
        scroll.setFillViewport(true);

        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setGravity(Gravity.CENTER_HORIZONTAL);
        layout.setPadding(40, 80, 40, 100);

        TextView title = new TextView(this);
        title.setText("Multi-Decompiler");
        title.setTextColor(Color.WHITE);
        title.setTextSize(28);
        title.setTypeface(null, Typeface.BOLD);
        title.setGravity(Gravity.CENTER);
        title.setShadowLayer(25, 0, 0, Color.CYAN);
        title.setPadding(20, 20, 20, 40);
        layout.addView(title);

        int screenWidth = getResources().getDisplayMetrics().widthPixels;
        int btnWidth = screenWidth - 80;

        Button apkBtn = new Button(this);
        apkBtn.setText("Decompile APK");
        applyMegaButtonStyle(apkBtn, "#FF0000", "#FF8C00", "#FFFF00", "#FFFFFF");
        LinearLayout.LayoutParams lp1 = new LinearLayout.LayoutParams(btnWidth, 160);
        lp1.bottomMargin = 30;
        layout.addView(apkBtn, lp1);
        apkBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(MainActivity.this, Jadx.class);
                i.putExtra("mode", "apk");
                startActivity(i);
            }
        });

        Button dexBtn = new Button(this);
        dexBtn.setText("Decompile DEX");
        applyMegaButtonStyle(dexBtn, "#00FFFF", "#0000FF", "#FFFFFF", "#FFFFFF");
        LinearLayout.LayoutParams lp2 = new LinearLayout.LayoutParams(btnWidth, 160);
        lp2.bottomMargin = 30;
        layout.addView(dexBtn, lp2);
        dexBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(MainActivity.this, Jadx.class);
                i.putExtra("mode", "dex");
                startActivity(i);
            }
        });

        Button smaliBtn = new Button(this);
        smaliBtn.setText("Smali to Java");
        applyMegaButtonStyle(smaliBtn, "#00FF00", "#008000", "#FFFFFF", "#FFFFFF");
        LinearLayout.LayoutParams lp3 = new LinearLayout.LayoutParams(btnWidth, 160);
        lp3.bottomMargin = 30;
        layout.addView(smaliBtn, lp3);
        smaliBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(MainActivity.this, SmaliToJava.class);
                startActivity(i);
            }
        });

        scroll.addView(layout);
        setContentView(scroll);
    }
}
