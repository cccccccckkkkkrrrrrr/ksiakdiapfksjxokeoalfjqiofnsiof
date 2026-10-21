package multi.decompiler.by.pbcat;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;

public class SmaliToJava extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        TextView tv = new TextView(this);
        tv.setText("Smali to Java требует отдельной библиотеки JDSXTool.\nЭтот экран в разработке.");
        tv.setTextColor(android.graphics.Color.WHITE);
        tv.setTextSize(18);
        tv.setPadding(40, 100, 40, 100);
        setContentView(tv);
        Toast.makeText(this, "В разработке", Toast.LENGTH_SHORT).show();
    }
}
