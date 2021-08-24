package com.example.marcinv.medicamentreminder;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class MinihraActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_minihra);
        android.support.v7.app.ActionBar actionBar = getSupportActionBar();
        actionBar.setTitle("Minihra");


        Button rebr = findViewById(R.id.rebricekMini);
        rebr.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                    Intent intent = new Intent(MinihraActivity.this, RebricekActivity.class);
                    startActivity(intent);

            }
        });


    }
}
