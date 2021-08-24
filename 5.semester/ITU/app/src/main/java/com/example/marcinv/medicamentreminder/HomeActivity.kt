package com.example.marcinv.medicamentreminder

import android.app.Notification
import android.app.PendingIntent
import android.app.PendingIntent.getActivity
import android.content.Intent
import android.os.Bundle
import android.os.CountDownTimer
import android.support.design.widget.BottomNavigationView
import android.support.design.widget.FloatingActionButton
import android.support.design.widget.Snackbar
import android.view.View
import android.support.design.widget.NavigationView
import android.support.v4.app.NotificationCompat
import android.support.v4.app.NotificationManagerCompat
import android.support.v4.content.ContextCompat
import android.support.v4.view.GravityCompat
import android.support.v4.widget.DrawerLayout
import android.support.v7.app.ActionBar
import android.support.v7.app.ActionBarDrawerToggle
import android.support.v7.app.AppCompatActivity
import android.support.v7.widget.Toolbar
import android.view.Menu
import android.view.MenuItem
import android.widget.EditText
import android.widget.ImageView
import android.widget.TextView
import com.example.marcinv.medicamentreminder.App.CHANNEL_1_ID
import com.example.marcinv.medicamentreminder.R.id.toolbar
import kotlinx.android.synthetic.main.activity_home.view.*


class HomeActivity : AppCompatActivity(), NavigationView.OnNavigationItemSelectedListener {

    private var notificationManager: NotificationManagerCompat? = null
    private var editTextTitle: EditText? = null
    private var editTextMessage: EditText? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_home)

        notificationManager = NotificationManagerCompat.from(this);
        // editTextTitle = findViewById(R.id.edit_text_title);
        //editTextMessage = findViewById(R.id.edit_text_message);


        notifMed()
        notifEx()

        val toolbar = findViewById<View>(R.id.toolbar) as Toolbar
        setSupportActionBar(toolbar)


        val drawer = findViewById<View>(R.id.drawer_layout) as DrawerLayout
        val toggle = ActionBarDrawerToggle(
            this, drawer, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close
        )
        drawer.addDrawerListener(toggle)
        toggle.syncState()

        val navigationView = findViewById<View>(R.id.nav_view) as NavigationView
        navigationView.setNavigationItemSelectedListener(this)
        navigationView.bringToFront()


        val header = navigationView.getHeaderView(0);
        val abc = header.findViewById<ImageView>(R.id.imageView)
        abc.setOnClickListener{
            val intent = Intent(this, ProfilActivity::class.java)
            startActivity(intent)
        }

        val name = header.findViewById<TextView>(R.id.name)
        name.setOnClickListener{
            val intent = Intent(this, ProfilActivity::class.java)
            startActivity(intent)
        }

        val fragmentHome = Domov()
        val domovTransaction = supportFragmentManager.beginTransaction()
        domovTransaction.replace(R.id.content, fragmentHome)
        domovTransaction.commit()

        toolbar.setNavigationIcon(R.mipmap.person)

        val mMainNav = findViewById<View>(R.id.main_nav) as BottomNavigationView
        mMainNav.setOnNavigationItemSelectedListener(BottomNavigationView.OnNavigationItemSelectedListener { menuItem ->
            when (menuItem.itemId) {
                R.id.nav_home -> {
                    val fragmentHome = Domov()
                    val domovTransaction = supportFragmentManager.beginTransaction()
                    domovTransaction.replace(R.id.content, fragmentHome)
                    domovTransaction.commit()
                    setTitle("MedRem")
                    return@OnNavigationItemSelectedListener true
                }
                R.id.nav_lieky -> {
                    val fragmentMedics = Lieky()
                    val liekyTransaction = supportFragmentManager.beginTransaction()
                    liekyTransaction.replace(R.id.content, fragmentMedics)
                    liekyTransaction.commit()
                    setTitle("Lieky")
                    return@OnNavigationItemSelectedListener true
                }
                R.id.nav_vysetrenia -> {
                    val fragmentTreats = Vysetrenia()
                    val vysetreniaTransaction = supportFragmentManager.beginTransaction()
                    vysetreniaTransaction.replace(R.id.content, fragmentTreats)
                    vysetreniaTransaction.commit()
                    setTitle("Vyšetrenia")
                    return@OnNavigationItemSelectedListener true
                }
                R.id.nav_viac -> {
                    val fragmentMore = Viac()
                    val viacTransaction = supportFragmentManager.beginTransaction()
                    viacTransaction.replace(R.id.content, fragmentMore)
                    viacTransaction.commit()
                    setTitle("Viac")
                    return@OnNavigationItemSelectedListener true
                }
            }
            false
        })

    }

    override fun onBackPressed() {
        val drawer = findViewById<View>(R.id.drawer_layout) as DrawerLayout
        if (drawer.isDrawerOpen(GravityCompat.START)) {
            drawer.closeDrawer(GravityCompat.START)
        } else {
            super.onBackPressed()
        }
    }


    override fun onNavigationItemSelected(item: MenuItem): Boolean {
        // Handle navigation view item clicks here.
        val id = item.itemId

        if (id == R.id.nav_friend) {
            // Handle nav_frien action
            val intent = Intent(this, PribuzniActivity::class.java)
            startActivity(intent)
        } else if (id == R.id.nav_rankings) {
            val intent = Intent(this, RebricekActivity::class.java)
            startActivity(intent)
        }

        val drawer = findViewById<View>(R.id.drawer_layout) as DrawerLayout
        drawer.closeDrawer(GravityCompat.START)
        return true
    }

    fun sendOnChannel1(v: View) {
        val title = editTextTitle?.getText().toString()
        val message = editTextMessage?.getText().toString()

        val activityIntent = Intent(this, HomeActivity::class.java)
        val contentIntent = PendingIntent.getActivity(this,
            0, activityIntent, 0)

        val broadcastIntent = Intent(this, NotificationReceiver::class.java)
        broadcastIntent.putExtra("toastMessage", message)
        val actionIntent = PendingIntent.getBroadcast(this,
            0, broadcastIntent, PendingIntent.FLAG_UPDATE_CURRENT)

        val notification = NotificationCompat.Builder(this, CHANNEL_1_ID)
            .setSmallIcon(R.mipmap.ic_med)
            .setContentTitle("Jozef, je čas užiť vaše lieky!")
            .setContentText("Codeine 1 tableta")
            .setPriority(NotificationCompat.PRIORITY_HIGH)
            .setCategory(NotificationCompat.CATEGORY_REMINDER)
            .setColor(ContextCompat.getColor(this, R.color.colorPrimary))
            .setContentIntent(contentIntent)
            .addAction(R.drawable.ic_add_black_24dp, "Odložiť", actionIntent)
            .addAction(R.mipmap.ic_launcher, "Užiť", actionIntent)
            .addAction(R.mipmap.ic_launcher, "Zrušiť", actionIntent)
            .build();

        notification.flags = notification.flags or Notification.FLAG_INSISTENT;

        notificationManager?.notify(1, notification)
    }

    fun sendOnChannel2(v: View) {
        //val title = editTextTitle?.getText().toString()
        val message = editTextMessage?.getText().toString()

        val activityIntent = Intent(this, HomeActivity::class.java)
        val contentIntent = PendingIntent.getActivity(this,
            0, activityIntent, 0)

        val broadcastIntent = Intent(this, NotificationReceiver::class.java)
        broadcastIntent.putExtra("toastMessage", message)
        val actionIntent = PendingIntent.getBroadcast(this,
            0, broadcastIntent, 0)

        val notification = NotificationCompat.Builder(this, CHANNEL_1_ID)
            .setSmallIcon(R.mipmap.ic_med)
            .setContentTitle("Jozef, oznámenie o vašom vyšetrení!")
            .setContentText("Neurológia, 11.12.2018, 11:00")
            .setPriority(NotificationCompat.PRIORITY_HIGH)
            .setCategory(NotificationCompat.CATEGORY_REMINDER)
            .setColor(ContextCompat.getColor(this, R.color.colorPrimary))
            .setContentIntent(contentIntent)
            .setOnlyAlertOnce(true)
            .addAction(R.mipmap.ic_launcher, "Zrušiť", actionIntent)
            .build();

        notificationManager?.notify(1, notification)
    }



    fun notifMed(){
        object : CountDownTimer(60000, 1000) {
            override fun onTick(millisUntilFinished: Long) {

            }
            override fun onFinish() {
                sendOnChannel1(window.currentFocus.rootView)
            }
        }.start()
    }

    fun notifEx(){
        object : CountDownTimer(120000, 1000) {
            override fun onTick(millisUntilFinished: Long) {

            }
            override fun onFinish() {
                sendOnChannel2(window.currentFocus.rootView)
            }
        }.start()
    }


}
