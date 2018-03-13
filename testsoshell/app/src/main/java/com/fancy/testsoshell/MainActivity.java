package com.fancy.testsoshell;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends Activity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("encrypto");
    }

    public native  String teststring();
    private TextView tvt;
    private EditText edt;
    private Button btn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

       // tv.setText(getString());
        tvt = (TextView) findViewById(R.id.test);
        edt =(EditText)findViewById(R.id.edit);
        btn = (Button)findViewById(R.id.bt1);

        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String letter=edt.getText().toString();
                if(!letter.isEmpty()){
                    if(letter.equals(teststring())){
                        tvt.setText("You got it!");
                    }
                }
            }
        });


    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */

    //public native String getString();
}

