package com.example.marcinv.medicamentreminder;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v7.app.AppCompatDialogFragment;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TimePicker;
import android.widget.Toast;


public class ExDialog extends AppCompatDialogFragment {

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        LayoutInflater inflater = getActivity().getLayoutInflater();
        View view = inflater.inflate(R.layout.layout_add_ex, null);

        TimePicker timePicker=(TimePicker)view.findViewById(R.id.addExTimepicker);
        timePicker.setIs24HourView(true);


        Spinner spinner = view.findViewById(R.id.addDocSpinner);
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(getContext(), R.array.list_add_doc_spinner, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinner.setAdapter(adapter);

        Spinner spinnerNotif = view.findViewById(R.id.exNotifSpinner);
        ArrayAdapter<CharSequence> adapterNotif = ArrayAdapter.createFromResource(getContext(), R.array.list_ex_notif_spinner, android.R.layout.simple_spinner_item);
        adapterNotif.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinnerNotif.setAdapter(adapterNotif);


        builder.setView(view);
        builder.setTitle("Pridanie nového vyšetrenie");
        builder.setNegativeButton("Zrušiť", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.cancel();
            }
        });

        builder.setPositiveButton("Pridať", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
            }

        });

        return builder.create();
    }
}
