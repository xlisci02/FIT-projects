package com.example.marcinv.medicamentreminder;


import android.content.Context;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.internal.NavigationMenu;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v4.app.Fragment;
import android.support.v7.widget.RecyclerView;
import android.text.Html;
import android.util.Log;
import android.util.TypedValue;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.*;
import com.prolificinteractive.materialcalendarview.CalendarDay;
import com.prolificinteractive.materialcalendarview.MaterialCalendarView;
import com.prolificinteractive.materialcalendarview.OnDateSelectedListener;
import com.prolificinteractive.materialcalendarview.OnMonthChangedListener;
import io.github.yavski.fabspeeddial.FabSpeedDial;
import org.threeten.bp.LocalDate;
import org.w3c.dom.Text;

import java.util.*;


/**
 * A simple {@link Fragment} subclass.
 */
public class Domov extends Fragment {
    public Domov() {}
    private TextView textDialog;

    @Override
    public  void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment

        View v = inflater.inflate(R.layout.fragment_domov, container, false);

        final FabSpeedDial fabSpeedDial = (FabSpeedDial)v.findViewById(R.id.fabSpeedDial);

        textDialog = (TextView) v.findViewById(R.id.AlertTextView);

        fabSpeedDial.setMenuListener(new FabSpeedDial.MenuListener() {
            @Override
            public boolean onPrepareMenu(NavigationMenu navigationMenu) {
                return true;
            }

            @Override
            public boolean onMenuItemSelected(MenuItem menuItem) {
                switch (menuItem.getItemId()){
                    case R.id.addMed:
                        addMedDialog();
                        break;
                    case R.id.addDoc:
                        addDocDialog();
                        break;
                    case R.id.addEx:
                        addExDialog();
                        break;
                }
                return true;
            }

            @Override
            public void onMenuClosed() {

            }
        });

        View button = v.findViewById(R.id.fab);
        (button).setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        Snackbar.make(v, "Replace with your own action", Snackbar.LENGTH_LONG).setAction("Action", null).show();
                    }
                }
        );



        View tv = v.findViewById(R.id.calendarView);
        ((MaterialCalendarView)tv).setSelectedDate(LocalDate.now());
        ((MaterialCalendarView)tv).setCurrentDate(LocalDate.now());

        ((MaterialCalendarView)tv).setOnDateChangedListener(new OnDateSelectedListener() {
            @Override
            public void onDateSelected(@NonNull MaterialCalendarView materialCalendarView, @NonNull CalendarDay calendarDay, boolean b) {
                if(calendarDay.getDay() %3 ==  0){
                   // Toast.makeText(getActivity(), "1", Toast.LENGTH_SHORT).show();
                }
                else if (calendarDay.getDay() %3 == 1 ){
                   // Toast.makeText(getActivity(), "2", Toast.LENGTH_SHORT).show();
                }
                else {
                   // Toast.makeText(getActivity(), "3", Toast.LENGTH_SHORT).show();
                }
            }
        });



        return v;
    }

    public void addMedDialog(){
        MedDialog medDialog = new MedDialog();
        medDialog.show(getFragmentManager(), "exampleeee");

    }

    public void addDocDialog(){
        DocDialog docDialog = new DocDialog();
        docDialog.show(getFragmentManager(), "exampleeee");

    }
    public void addExDialog(){
        ExDialog exDialog = new ExDialog();
        exDialog.show(getFragmentManager(), "exampleeee");

    }


    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        List<String> listDataHeader = new ArrayList<>();
        String[] mItemHeaders = getResources().getStringArray(R.array.items_array_expandable_level_one);
        Collections.addAll(listDataHeader, mItemHeaders);
        final ExpandableListView mExpandableListView = (ExpandableListView) view.findViewById(R.id.expandableListView_Parent);

        if (mExpandableListView != null) {
            ParentLevelAdapter parentLevelAdapter = new ParentLevelAdapter(view.getContext(), listDataHeader);
            mExpandableListView.setAdapter(parentLevelAdapter);
            // display only one expand item
//            mExpandableListView.setOnGroupExpandListener(new ExpandableListView.OnGroupExpandListener() {
//                int previousGroup = -1;
//                @Override
//                public void onGroupExpand(int groupPosition) {
//                    if (groupPosition != previousGroup)
//                        mExpandableListView.collapseGroup(previousGroup);
//                    previousGroup = groupPosition;
//                }
//            });
        }

    }


    public class CustomExpListView extends ExpandableListView
    {
        public CustomExpListView(Context context)
        {
            super(context);
        }



        protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec)
        {
            widthMeasureSpec = MeasureSpec.makeMeasureSpec(980, MeasureSpec.AT_MOST);
            heightMeasureSpec = MeasureSpec.makeMeasureSpec(20000, MeasureSpec.AT_MOST);
            super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        }

    }

    public class ParentLevelAdapter extends BaseExpandableListAdapter {
        private final Context mContext;
        private final List<String> mListDataHeader;
        private final Map<String, List<String>> mListData_SecondLevel_Map;
        private final Map<String, List<String>> mListData_ThirdLevel_Map;
        private final LayoutInflater inf;

        public ParentLevelAdapter(Context mContext, List<String> mListDataHeader) {
            inf = LayoutInflater.from(getActivity());
            this.mContext = mContext;
            this.mListDataHeader = new ArrayList<>();
            this.mListDataHeader.addAll(mListDataHeader);
            // SECOND LEVEL
            String[] mItemHeaders;
            mListData_SecondLevel_Map = new HashMap<>();
            int parentCount = mListDataHeader.size();
            for (int i = 0; i < parentCount; i++) {
                String content = mListDataHeader.get(i);
                switch (content) {
                    case "8:00":
                        mItemHeaders = mContext.getResources().getStringArray(R.array.items_array_expandable_level_one_one_child);
                        break;
                    case "12:00":
                        mItemHeaders = mContext.getResources().getStringArray(R.array.items_array_expandable_level_one_two_child);
                        break;
                    default:
                        mItemHeaders = mContext.getResources().getStringArray(R.array.items_array_expandable_level_three);
                }
                mListData_SecondLevel_Map.put(mListDataHeader.get(i), Arrays.asList(mItemHeaders));
            }
            // THIRD LEVEL
            String[] mItemChildOfChild;
            List<String> listChild;
            mListData_ThirdLevel_Map = new HashMap<>();
            for (Object o : mListData_SecondLevel_Map.entrySet()) {
                Map.Entry entry = (Map.Entry) o;
                Object object = entry.getValue();
                if (object instanceof List) {
                    List<String> stringList = new ArrayList<>();
                    Collections.addAll(stringList, (String[]) ((List) object).toArray());
                    for (int i = 0; i < stringList.size(); i++) {
                        mItemChildOfChild = mContext.getResources().getStringArray(R.array.items_array_expandable_level_three);
                        listChild = Arrays.asList(mItemChildOfChild);
                        mListData_ThirdLevel_Map.put(stringList.get(i), listChild);
                    }
                }
            }
        }

        @Override
        public Object getChild(int groupPosition, int childPosition) {
            return childPosition;
        }

        @Override
        public long getChildId(int groupPosition, int childPosition) {
            return childPosition;
        }

        @Override
        public View getChildView(int groupPosition, int childPosition,
                                 boolean isLastChild, View convertView, ViewGroup parent) {
        String headerTitle = (String) getGroup(groupPosition);


        final CustomExpListView secondLevelExpListView = new CustomExpListView(this.mContext);
        secondLevelExpListView.setDivider(null);
        String parentNode = (String) getGroup(groupPosition);
        secondLevelExpListView.setAdapter(new SecondLevelAdapter(this.mContext, mListData_SecondLevel_Map.get(parentNode), mListData_ThirdLevel_Map));
        //        secondLevelExpListView.setOnGroupExpandListener(new ExpandableListView.OnGroupExpandListener() {
        //            int previousGroup = -1;
        //            @Override
        //            public void onGroupExpand(int groupPosition) {
        //                if (groupPosition != previousGroup)
        //                    secondLevelExpListView.collapseGroup(previousGroup);
        //                previousGroup = groupPosition;
        //            }
        //        });
        if(String.valueOf(headerTitle.charAt(0)) == "5" ||
                String.valueOf(headerTitle.charAt(0)) == "6") {
            secondLevelExpListView.setGroupIndicator(null);
        }
        return secondLevelExpListView;

        }

        @Override
        public int getChildrenCount(int groupPosition) {
            return 1;
        }

        @Override
        public Object getGroup(int groupPosition) {
            return this.mListDataHeader.get(groupPosition);
        }

        @Override
        public int getGroupCount() {
            return this.mListDataHeader.size();
        }

        @Override
        public long getGroupId(int groupPosition) {
            return groupPosition;
        }

        @Override
        public View getGroupView(int groupPosition, boolean isExpanded,
                                 View convertView, ViewGroup parent) {
            String headerTitle = (String) getGroup(groupPosition);
            if (convertView == null) {
                convertView = inf.inflate(R.layout.homeitem_header, parent, false);
            }
            TextView lblListHeader = (TextView) convertView
                    .findViewById(R.id.lblHomeItemHeader);
            lblListHeader.setTypeface(null, Typeface.BOLD);
            //lblListHeader.setTextColor(Color.CYAN);
            lblListHeader.setText(headerTitle);
            return convertView;
        }

        @Override
        public boolean hasStableIds() {
            return true;
        }

        @Override
        public boolean isChildSelectable(int groupPosition, int childPosition) {
            return true;
        }
    }

    public class SecondLevelAdapter extends BaseExpandableListAdapter {
        private final Context mContext;
        private final List<String> mListDataHeader;
        private final Map<String, List<String>> mListDataChild;
        private final LayoutInflater inf;

        public SecondLevelAdapter(Context mContext, List<String> mListDataHeader, Map<String, List<String>> mListDataChild) {
            this.mContext = mContext;
            this.mListDataHeader = mListDataHeader;
            this.mListDataChild = mListDataChild;
            inf = LayoutInflater.from(getActivity());

        }

        @Override
        public Object getChild(int groupPosition, int childPosition) {
            return this.mListDataChild.get(this.mListDataHeader.get(groupPosition))
                    .get(childPosition);
        }

        @Override
        public long getChildId(int groupPosition, int childPosition) {
            return childPosition;
        }

        @Override
        public View getChildView(int groupPosition, int childPosition,
                                 boolean isLastChild, View convertView, ViewGroup parent) {
            if(groupPosition == 1) {
                convertView = inf.inflate(R.layout.empty_info, parent, false);
            }
            else {
                convertView = inf.inflate(R.layout.homeitem_info, parent, false);
            }
            return convertView;
        }

        @Override
        public int getChildrenCount(int groupPosition) {
            try {
                return this.mListDataChild.get(this.mListDataHeader.get(groupPosition)).size();
            } catch (Exception e) {
                return 0;
            }
        }

        @Override
        public Object getGroup(int groupPosition) {
            return this.mListDataHeader.get(groupPosition);
        }

        @Override
        public int getGroupCount() {
            return this.mListDataHeader.size();
        }

        @Override
        public long getGroupId(int groupPosition) {
            return groupPosition;
        }

        @Override
        public View getGroupView(int groupPosition, boolean isExpanded,
                                 View convertView, ViewGroup parent) {
            String headerTitle = (String) getGroup(groupPosition);
            if (convertView == null) {
                convertView = inf.inflate(R.layout.medication_header, parent, false);
            }
            TextView lblListHeader = (TextView) convertView
                    .findViewById(R.id.lblMedicationItemHeader);

            switch (String.valueOf(headerTitle.charAt(0))) {
                case "1":
                    lblListHeader.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.tablet2,0,0,0);
                    break;
                case "2":
                    lblListHeader.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.tablet1,0,0,0);
                    break;
                case "3":
                    lblListHeader.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.tablet3,0,0,0);
                    break;
                case "4":
                    lblListHeader.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.tablet4,0,0,0);
                    break;
                case "5":
                    lblListHeader.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.teeth,0,0,0);
                    break;
                case "6":
                    lblListHeader.setCompoundDrawablesWithIntrinsicBounds(R.mipmap.eye,0,0,0);
                    break;

            }
            lblListHeader.setText(Html.fromHtml(headerTitle.substring(1,headerTitle.length())));
            return convertView;
        }

        @Override
        public boolean hasStableIds() {
            return true;
        }

        @Override
        public boolean isChildSelectable(int groupPosition, int childPosition) {
            return true;
        }
    }
}
