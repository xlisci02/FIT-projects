package com.example.marcinv.medicamentreminder;


import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.text.Html;
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
public class Vysetrenia extends Fragment {
    View rootView;
    ExpandableListView lv;

    private List<String> listDataHeader;
    private HashMap<Integer, HashMap<Integer, String>> outerHash;

    public Vysetrenia() {

    }


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
        lv.setAdapter(new ExpandableListAdapterVysetrenia(listDataHeader,outerHash));
        //lv.setGroupIndicator(null);

    }

    private void initData() {

        outerHash = new HashMap<Integer, HashMap<Integer, String>>();
        listDataHeader = new ArrayList<>();

        addInnerHash(Arrays.asList("CT", "Ján Novák", "23.4.20018, 14:00", "Nova 3, Bratislava"));
        addInnerHash(Arrays.asList("EKG", "Ján Janoska", "30.10.2018, 9:30","Stara 1, Bratislava"));
        addInnerHash(Arrays.asList("Preventívna prehliaka", "Monika Šťastná", "21.3.2018, 8:00","Stara 6, Bratislava"));
        addInnerHash(Arrays.asList("Zmena dioptrií", "Marta Kutna", "5.11.2013, 12:30","Nova 9, Bratislava"));

        Integer counter = 0;
        for(HashMap.Entry<Integer, HashMap<Integer, String>> entry : outerHash.entrySet()) {
            listDataHeader.add("<b>" +outerHash.get(counter).get(0)+ "</b>" + "\n" + "<span style=\"color:#D81B60\"><br>" + outerHash.get(counter).get(2) + "</br></span>");
            counter++;
        }
    }

    private void addInnerHash(List<String> list){
        HashMap<Integer, String> innerHash = new HashMap<Integer, String>();
        for (int i=0; i<4; i++){
            innerHash.put(i, list.get(i));
        }

        Integer lastIndex = 0;
        for(HashMap.Entry<Integer, HashMap<Integer, String>> entry : outerHash.entrySet()) {
            lastIndex++;
        }
        outerHash.put(lastIndex, innerHash);
    }

    public class ExpandableListAdapterVysetrenia extends BaseExpandableListAdapter {
        private final LayoutInflater inf;
        private List<String> listDataHeader;
        private HashMap<Integer, HashMap<Integer, String>> outerHash;

        public ExpandableListAdapterVysetrenia(List<String> listDataHeader, HashMap<Integer, HashMap<Integer, String>> outerHash) {
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
                convertView = inf.inflate(R.layout.info_vysetrenie, parent, false);
                holder = new ViewHolder();

                holder.text = (TextView) convertView.findViewById(R.id.lblListItem2);
                convertView.setTag(holder);
            } else {
                holder = (ViewHolder) convertView.getTag();
            }

            final String meno = (String)getChild(groupPosition,0);
            final String spec = (String)getChild(groupPosition,1);
            final String telc = (String)getChild(groupPosition, 2);
            final String email = (String)getChild(groupPosition,3);


            TextView txtMeno = (TextView)convertView.findViewById(R.id.lblListItem011);
            TextView txtSpec= (TextView)convertView.findViewById(R.id.lblListItem111);
            TextView txtTelc= (TextView)convertView.findViewById(R.id.lblListItem211);
            TextView txtEmail= (TextView)convertView.findViewById(R.id.lblListItem311);



            txtMeno.setText(meno);
            txtSpec.setText(spec);
            txtTelc.setText(telc);
            txtEmail.setText(email);


            return convertView;
        }

        @Override
        public View getGroupView(int groupPosition, boolean isExpanded, View convertView, ViewGroup parent) {
            ViewHolder holder;

            if (convertView == null) {
                convertView = inf.inflate(R.layout.header_vysetrenia, parent, false);

                holder = new ViewHolder();
                holder.text = (TextView) convertView.findViewById(R.id.lblListHeader2);
                convertView.setTag(holder);
            } else {
                holder = (ViewHolder) convertView.getTag();
            }

            holder.text.setText(Html.fromHtml(getGroup(groupPosition).toString()));
            if(groupPosition == 1)
                holder.text.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.ekg, 0,0,0);
            else if(groupPosition == 0)
                holder.text.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.ct, 0,0,0);
            else if(groupPosition == 2)
                holder.text.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.teeth, 0,0,0);
            else if(groupPosition == 3)
                holder.text.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.eye, 0,0,0);

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