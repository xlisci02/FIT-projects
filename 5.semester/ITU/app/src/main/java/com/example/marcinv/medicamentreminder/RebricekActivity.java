package com.example.marcinv.medicamentreminder;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class RebricekActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_rebricek);
        android.support.v7.app.ActionBar actionBar = getSupportActionBar();
        actionBar.setTitle("Rebríček");

    }
}
