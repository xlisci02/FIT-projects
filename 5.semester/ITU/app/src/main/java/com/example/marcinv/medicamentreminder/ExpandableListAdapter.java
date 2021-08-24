package com.example.marcinv.medicamentreminder;

import android.content.Context;
import android.graphics.Typeface;
import android.text.Html;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.TextView;

import java.util.HashMap;
import java.util.List;

/**
 * Created by reale on 21/11/2016.
 */

public class ExpandableListAdapter extends BaseExpandableListAdapter {
    private Context context;
    private List<String> listDataHeader;
    private HashMap<Integer, HashMap<Integer, String>> outerHash;

    public ExpandableListAdapter(Context context, List<String> listDataHeader, HashMap<Integer, HashMap<Integer, String>> outerHash) {
        this.context = context;
        this.listDataHeader = listDataHeader;
        this.outerHash = outerHash;
    }

    @Override
    public int getGroupCount() {
        return listDataHeader.size();
    }

    @Override
    public int getChildrenCount(int i) {
        return 1;
        //return 0;
    }

    @Override
    public Object getGroup(int i) {
        return listDataHeader.get(i);
    }

    @Override
    public Object getChild(int i, int tableIndex) {
        return outerHash.get(i).get(tableIndex); // i = Group Item , i1 = tableIndex
        //return 0;
    }

    @Override
    public long getGroupId(int i) {
        return i;
    }

    @Override
    public long getChildId(int i, int i1) {
        return i1;
    }

    @Override
    public boolean hasStableIds() {
        return false;
    }

    @Override
    public View getGroupView(int i, boolean b, View view, ViewGroup viewGroup) {
        String headerTitle = (String)getGroup(i);
        if(view == null)
        {
            LayoutInflater inflater = (LayoutInflater)this.context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            view = inflater.inflate(R.layout.list_group,null);
        }
        TextView lblListHeader = (TextView)view.findViewById(R.id.lblListHeader);
        //lblListHeader.setTypeface(null, Typeface.BOLD);
        lblListHeader.setText(Html.fromHtml(headerTitle));

        switch (i) {
            case 0:
            case 2:
                lblListHeader.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.doc,0,0,0);
                break;
            case 1:
            case 3:
                lblListHeader.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.docw,0,0,0);
                break;
        }

        return view;
    }

    @Override
    public View getChildView(int i, int i1, boolean b, View view, ViewGroup viewGroup) {

        if(view == null)
        {
            LayoutInflater inflater = (LayoutInflater)this.context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            view = inflater.inflate(R.layout.list_item,null);
        }


        final String meno = (String)getChild(i,0);
        final String spec = (String)getChild(i,1);
        final String telc = (String)getChild(i, 2);
        final String email = (String)getChild(i,3);
        final String adresa = (String)getChild(i,4);

        TextView txtMeno = (TextView)view.findViewById(R.id.lblListItem0);
        TextView txtSpec= (TextView)view.findViewById(R.id.lblListItem1);
        TextView txtTelc= (TextView)view.findViewById(R.id.lblListItem2);
        TextView txtEmail= (TextView)view.findViewById(R.id.lblListItem3);
        TextView txtAdresa= (TextView)view.findViewById(R.id.lblListItem4);

        txtMeno.setText(meno);
        txtSpec.setText(spec);
        txtTelc.setText(telc);
        txtEmail.setText(email);
        txtAdresa.setText(adresa);

        return view;
    }

    @Override
    public boolean isChildSelectable(int i, int i1) {
        return true;
    }
}
