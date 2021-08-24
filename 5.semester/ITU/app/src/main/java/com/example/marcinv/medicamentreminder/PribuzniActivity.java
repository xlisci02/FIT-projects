package com.example.marcinv.medicamentreminder;

import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.Html;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.Button;
import android.widget.ExpandableListView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

public class PribuzniActivity extends AppCompatActivity {
    View rootView;
    ExpandableListView lv;

    private List<String> listDataHeader;
    private HashMap<Integer, HashMap<Integer, String>> outerHash;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pribuzni);
        Log.d("cccc", "aahooj");
        lv = (ExpandableListView) findViewById(R.id.liekyExp);
        initData();
        lv.setAdapter(new ExpandableListAdapterLieky(listDataHeader,outerHash));

        android.support.v7.app.ActionBar actionBar = getSupportActionBar();
        actionBar.setTitle("Moji priatelia");


        FloatingActionButton asd = findViewById(R.id.addFriend);
        asd.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                addDialog dialog = new addDialog();
                dialog.show(getSupportFragmentManager(),"ahoj");

            }
        });

    }

    private void initData() {

        outerHash = new HashMap<Integer, HashMap<Integer, String>>();
        listDataHeader = new ArrayList<>();

        addInnerHash(Arrays.asList("Peter Suchý", "sucho13", "45"));
        addInnerHash(Arrays.asList("Roland Novák", "novakrolo11", "64"));
        addInnerHash(Arrays.asList("Roman Jeleň", "jelen120", "67"));
        addInnerHash(Arrays.asList("Lukáš Kucko", "kucoluky5", "59"));



        Integer counter = 0;
        for(HashMap.Entry<Integer, HashMap<Integer, String>> entry : outerHash.entrySet()) {
            listDataHeader.add("<b>" + outerHash.get(counter).get(0) + "</b>");
            counter++;
        }
    }

    private void addInnerHash(List<String> list){
        HashMap<Integer, String> innerHash = new HashMap<Integer, String>();
        for (int i=0; i<3; i++){
            innerHash.put(i, list.get(i));
        }

        Integer lastIndex = 0;
        for(HashMap.Entry<Integer, HashMap<Integer, String>> entry : outerHash.entrySet()) {
            lastIndex++;
        }
        outerHash.put(lastIndex, innerHash);
    }

    public class ExpandableListAdapterLieky extends BaseExpandableListAdapter {
        private final LayoutInflater inf;
        private List<String> listDataHeader;
        private HashMap<Integer, HashMap<Integer, String>> outerHash;

        public ExpandableListAdapterLieky(List<String> listDataHeader, HashMap<Integer, HashMap<Integer, String>> outerHash) {
            this.listDataHeader = listDataHeader;
            this.outerHash = outerHash;
            inf = LayoutInflater.from(PribuzniActivity.this);
        }

        @Override
        public int getGroupCount() {
            return listDataHeader.size();
        }

        @Override
        public int getChildrenCount(int groupPosition) {
            return 1;
        }

        @Override
        public Object getGroup(int groupPosition) {
            return listDataHeader.get(groupPosition);
        }

        @Override
        public Object getChild(int groupPosition, int childPosition) {
            return outerHash.get(groupPosition).get(childPosition);
        }

        @Override
        public long getGroupId(int groupPosition) {
            return groupPosition;
        }

        @Override
        public long getChildId(int groupPosition, int childPosition) {
            return childPosition;
        }

        @Override
        public boolean hasStableIds() {
            return true;
        }

        @Override
        public View getChildView(int groupPosition, final int childPosition, boolean isLastChild, View convertView, ViewGroup parent) {

            ViewHolder holder;
            if (convertView == null) {
                convertView = inf.inflate(R.layout.info_pribuzni, parent, false);
                holder = new ViewHolder();

                holder.text = (TextView) convertView.findViewById(R.id.lblListItem11);
                convertView.setTag(holder);
            } else {
                holder = (ViewHolder) convertView.getTag();
            }

            final String meno = (String)getChild(groupPosition,0);
            final String spec = (String)getChild(groupPosition,1);
            final String telc = (String)getChild(groupPosition, 2);
            final String email = (String)getChild(groupPosition,3);
            final String adresa = (String)getChild(groupPosition,4);
            final String adresa1 = (String)getChild(groupPosition,5);

            TextView txtSpec= (TextView)convertView.findViewById(R.id.lblListItem11);
            TextView txtTelc= (TextView)convertView.findViewById(R.id.lblListItem21);


            txtSpec.setText(spec);
            txtTelc.setText(telc);


            return convertView;
        }

        @Override
        public View getGroupView(int groupPosition, boolean isExpanded, View convertView, ViewGroup parent) {
            ViewHolder holder;

            if (convertView == null) {
                convertView = inf.inflate(R.layout.header_pribuzni, parent, false);

                holder = new ViewHolder();
                holder.text = (TextView) convertView.findViewById(R.id.lblListHeader1);
                convertView.setTag(holder);
            } else {
                holder = (ViewHolder) convertView.getTag();
            }

            holder.text.setText(Html.fromHtml(getGroup(groupPosition).toString()));
            if(groupPosition == 1)
                holder.text.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.user, 0,0,0);
            else if(groupPosition == 0)
                holder.text.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.old, 0,0,0);
            else if(groupPosition == 2)
                holder.text.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.user, 0,0,0);
            else if(groupPosition == 3)
                holder.text.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.old, 0,0,0);


            return convertView;
        }

        @Override
        public boolean isChildSelectable(int groupPosition, int childPosition) {
            return true;
        }

        private class ViewHolder {
            TextView text;
        }
    }
}
