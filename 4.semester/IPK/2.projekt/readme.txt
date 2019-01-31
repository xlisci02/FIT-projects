Autor: Matus Liscinsky
Login: xlisci02
Datum: 9.4.2018

IPK 2.Projekt - DHCP Starvation attack
Popis: Aplikácia realizujíca DHCP Starvation útok, ktorý 
vyčerpe adresný pool legitimneho DHCP servera

build:
    $ make
    $ make all

run:
    $ sudo ./ipk-dhcpstarve -i <interface>

Be careful.   