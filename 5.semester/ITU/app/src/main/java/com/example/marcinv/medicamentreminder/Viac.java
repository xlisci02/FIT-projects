package com.example.marcinv.medicamentreminder;


import android.content.Intent;
import android.os.Bundle;
import android.support.design.widget.NavigationView;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.*;
import android.widget.Button;
import android.widget.TextView;

import java.util.zip.Inflater;


/**
 * A simple {@link Fragment} subclass.
 */
public class Viac extends Fragment {
    private TextView settings;
    private TextView docs;
    private TextView minihra;

    public Viac() {
        // Required empty public constructor
    }




    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment

        View view = inflater.inflate(R.layout.fragment_viac, container, false);

        settings = (TextView) view.findViewById(R.id.settings);
        //settings.setCompoundDrawablesWithIntrinsicBounds(R.drawable.ic_build_black_24dp, 0,0,0);
        settings.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                opensettings();
            }
        });

        docs = (TextView) view.findViewById(R.id.docs);
        docs.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                openlekari();
            }
        });

        minihra = (TextView) view.findViewById(R.id.game);

        minihra.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                openminihra();
            }
        });

        return view;


    }



    public void opensettings() {
        Intent intent = new Intent(getActivity(), SettingsActivity.class);
        startActivity(intent);
    }

    public void openlekari() {
        Intent intent = new Intent(getActivity(), Lekari.class);
        startActivityForResult(intent, 5);
    }

    public void openminihra() {
        Intent intent = new Intent(getActivity(), MinihraActivity.class);
        startActivityForResult(intent, 5);
    }

}
