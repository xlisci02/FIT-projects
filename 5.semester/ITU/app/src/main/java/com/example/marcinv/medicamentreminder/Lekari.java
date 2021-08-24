package com.example.marcinv.medicamentreminder;
import android.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.MenuItem;
import android.view.ViewDebug;
import android.widget.ExpandableListView;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

public class Lekari extends AppCompatActivity {
    private ExpandableListView listView;
    private ExpandableListAdapter listAdapter;
    private List<String> listDataHeader;
    private HashMap<String,List<String>> listHash;
    private HashMap<Integer, HashMap<Integer, String>> outerHash;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_lekari);

        android.support.v7.app.ActionBar actionBar = getDelegate().getSupportActionBar();
        actionBar.setTitle("Lekári");


        listView = (ExpandableListView)findViewById(R.id.lvExp);
        initData();
        listAdapter = new ExpandableListAdapter(this,listDataHeader,outerHash);
        listView.setAdapter(listAdapter);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            onBackPressed();
            return true;
        }
        return false;
    }

    @Override
    public void onBackPressed() {
        this.finish();
    }

    private void initData() {

        outerHash = new HashMap<Integer, HashMap<Integer, String>>();
        listDataHeader = new ArrayList<>();

        addInnerHash(Arrays.asList("Ján Novák", "Neurológ", "+421 948 514 321", "novak@gmail.com", "Nova 3, Bratislava"));
        addInnerHash(Arrays.asList("Monika Šťastná", "Stomatológ", "+421 948 544 321", "stastnam@gmail.com", "Stara 6, Bratislava"));
        addInnerHash(Arrays.asList("Ján Janoska", "Kardiológ", "+421 948 444 321", "janoska@gmail.com", "Stara 1, Bratislava"));
        addInnerHash(Arrays.asList("Marta Kutná", "Oftalmológ", "+421 958 334 321", "kutna@gmail.com", "Nova 9, Bratislava"));

        Integer counter = 0;
        for(HashMap.Entry<Integer, HashMap<Integer, String>> entry : outerHash.entrySet()) {
            listDataHeader.add("<b>" + outerHash.get(counter).get(0) + "</b>" + "\n" + "<span style=\"color:#D81B60\"><br>" + outerHash.get(counter).get(1) + "</br></span>");
            counter++;
        }
    }

    private void addInnerHash(List<String> list){
        HashMap<Integer, String> innerHash = new HashMap<Integer, String>();
        for (int i=0; i<5; i++){
            innerHash.put(i, list.get(i));
        }

        Integer lastIndex = 0;
        for(HashMap.Entry<Integer, HashMap<Integer, String>> entry : outerHash.entrySet()) {
            lastIndex++;
        }
        outerHash.put(lastIndex, innerHash);
    }
}