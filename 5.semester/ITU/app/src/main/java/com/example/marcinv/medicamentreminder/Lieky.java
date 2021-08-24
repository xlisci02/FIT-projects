package com.example.marcinv.medicamentreminder;


import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.text.Html;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.*;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;


/**
 * A simple {@link Fragment} subclass.
 */
public class Lieky extends Fragment {
    View rootView;
    ExpandableListView lv;

    private List<String> listDataHeader;
    private HashMap<Integer, HashMap<Integer, String>> outerHash;

    public Lieky() {}


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        rootView = inflater.inflate(R.layout.fragment_lieky, container, false);

        return rootView;
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        lv = (ExpandableListView) view.findViewById(R.id.liekyExp);
        initData();
        lv.setAdapter(new ExpandableListAdapterLieky(listDataHeader,outerHash));
        //lv.setGroupIndicator(null);

    }

    private void initData() {

        outerHash = new HashMap<Integer, HashMap<Integer, String>>();
        listDataHeader = new ArrayList<>();

        addInnerHash(Arrays.asList("Complete Dual Act Chw Tab", "12.12.2018, 8:00", "ut, pi", "8:00/1krát denne/2ks","30.10.2018","-"));
        addInnerHash(Arrays.asList("Panadol 500", "11.12.2018, 10:00", "každý deň", "8:00/5krát denne/1ks", "30.11.2018","-"));
        addInnerHash(Arrays.asList("Codeine", "12.12.2018, 8:00", "st, ne", "8:00/1krát denne/2ks","10.6.2017","-"));
        addInnerHash(Arrays.asList("Euthyrox", "12.12.2018, 8:00", "každý deň", "8:00/1krát denne/1ks","30.10.2016","-"));



        Integer counter = 0;
        for(HashMap.Entry<Integer, HashMap<Integer, String>> entry : outerHash.entrySet()) {
            listDataHeader.add("<b>" + outerHash.get(counter).get(0) + "</b>");
            counter++;
        }
    }

    private void addInnerHash(List<String> list){
        HashMap<Integer, String> innerHash = new HashMap<Integer, String>();
        for (int i=0; i<6; i++){
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
            inf = LayoutInflater.from(getActivity());
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
                convertView = inf.inflate(R.layout.info_liek, parent, false);
                holder = new ViewHolder();

                holder.text = (TextView) convertView.findViewById(R.id.lblListItem1);
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

            TextView txtMeno = (TextView)convertView.findViewById(R.id.lblListItem01);
            TextView txtSpec= (TextView)convertView.findViewById(R.id.lblListItem11);
            TextView txtTelc= (TextView)convertView.findViewById(R.id.lblListItem21);
            TextView txtEmail= (TextView)convertView.findViewById(R.id.lblListItem31);
            TextView txtAdresa= (TextView)convertView.findViewById(R.id.lblListItem41);
            TextView txtAdresa1= (TextView)convertView.findViewById(R.id.lblListItem51);


            txtMeno.setText(meno);
            txtSpec.setText(spec);
            txtTelc.setText(telc);
            txtEmail.setText(email);
            txtAdresa.setText(adresa);
            txtAdresa1.setText(adresa1);

            return convertView;
        }

        @Override
        public View getGroupView(int groupPosition, boolean isExpanded, View convertView, ViewGroup parent) {
            ViewHolder holder;

            if (convertView == null) {
                convertView = inf.inflate(R.layout.header_lieky, parent, false);

                holder = new ViewHolder();
                holder.text = (TextView) convertView.findViewById(R.id.lblListHeader1);
                convertView.setTag(holder);
            } else {
                holder = (ViewHolder) convertView.getTag();
            }

            holder.text.setText(Html.fromHtml(getGroup(groupPosition).toString()));
            if(groupPosition == 1)
                holder.text.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.tablet2, 0,0,0);
            else if(groupPosition == 0)
                holder.text.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.tablet1, 0,0,0);
            else if(groupPosition == 2)
                holder.text.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.tablet3, 0,0,0);
            else if(groupPosition == 3)
                holder.text.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.tablet4, 0,0,0);


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
