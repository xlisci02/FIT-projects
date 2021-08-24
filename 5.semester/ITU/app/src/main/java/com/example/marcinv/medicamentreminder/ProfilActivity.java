package com.example.marcinv.medicamentreminder;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class ProfilActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_profil);
        android.support.v7.app.ActionBar actionBar = getSupportActionBar();
        actionBar.setTitle("Profil používateľa");

        Button edit = findViewById(R.id.editBtn);
        edit.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                editDialog dialog = new editDialog();
                dialog.show(getSupportFragmentManager(),"ahoj");

            }
        });
    }
}
