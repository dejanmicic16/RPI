#include "dialog.h"
#include "ui_dialog.h"

int MOTORONOFF = 0;
int SMER = 0;

int dht11_dat[5] = { 0, 0, 0, 0, 0 };

int speed = 1000;

int temperature = 0;
int temperature_gr = 25;
QString temperature_str;
QString temperature_gr_str;

int huminity = 0;
int huminity_gr = 25;

QString huminity_str;
QString huminity_gr_str;

uint8_t laststate	= HIGH;
uint8_t counter		= 0;

int GrM_A = 0;
int GrO_F = 0;
int VnM_A = 0;
int VnO_F = 0;

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    
    QTimer *myTimer1 = new QTimer(this);
    connect(myTimer1, SIGNAL(timeout()), this, SLOT(ReadTimeSlot()));

    myTimer1->start(500);
    
    ui->lineEdit_3->setText("Temperatura je :");
    ui->lineEdit_4->setText("Vlaznost je :");



}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::ReadTimeSlot()
{

    if ( wiringPiSetup() == -1 ) //Ukljucivanje i podesavanje WiringPi
       exit (1);
       
    senzor(); // Ocitavanje trenutne temperature i vlaznosti
    
    pinMode( DIRECTION, OUTPUT ); // Podesavanja za motor
    pinMode( STEP, OUTPUT );
    digitalWrite( DIRECTION, SMER );
    
	int speed_s = ui->verticalSlider->value();
    speed = MAXSPEED + speed_s * 4;
    
    for(int i =0; i<200; i++)
    {
        motor();
    }

}

void Dialog::motor()
{
    digitalWrite( STEP, MOTORONOFF  );
    delayMicroseconds( speed );
    digitalWrite( STEP, LOW );
    delayMicroseconds( speed / 2);
}

void Dialog::senzor()
{
    ui->lineEdit_5->setText("Granica za temperaturu je: ");
    ui->lineEdit_7->setText("Granica za vlaznost  je: ");

    temperature_gr_str = ui->lineEdit_6->text();
    temperature_gr = temperature_gr_str.toInt();

    huminity_gr_str = ui->lineEdit_8->text();
    huminity_gr = huminity_gr_str.toInt();

    //qDebug() <<"temperatura je:" << temperature_gr << Qt::endl;
    //qDebug() <<"Vlaznost je:" << huminity_gr <<Qt::endl;


    uint8_t j		= 0, i;
    //float	f;

    dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

    pinMode( DHTPIN, OUTPUT );
    digitalWrite( DHTPIN, LOW );
    delay( 18 );
    digitalWrite( DHTPIN, HIGH );
    delayMicroseconds( 40 );
    pinMode( DHTPIN, INPUT );

    for ( i = 0; i < MAXTIMINGS; i++ )
    {
        counter = 0;
        while ( digitalRead( DHTPIN ) == laststate )
        {
            counter++;
            delayMicroseconds( 1 );
            if ( counter == 255 )
            {
                break;
            }
        }
        laststate = digitalRead( DHTPIN );

        if ( counter == 255 )
            break;

        if ( (i >= 4) && (i % 2 == 0) )
        {
            dht11_dat[j / 8] <<= 1;
            if ( counter > 16 )
                dht11_dat[j / 8] |= 1;
            j++;
        }
    }

    if ( (j >= 40) &&
         (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
    {
        temperature = dht11_dat[2] + dht11_dat[3] / 10;
        temperature_str = QString::number(temperature);

        huminity = dht11_dat[0]+ dht11_dat[1] / 10;
        huminity_str = QString::number(huminity);

        ui->lineEdit->setText(temperature_str);
        ui->lineEdit_2->setText(huminity_str);


        //qDebug() <<"temperatura je:" << temperature << Qt::endl;
        //qDebug() <<"Vlaznost je:" << huminity <<Qt::endl;

    }
    else // sluzilo za debug
    {
        //qDebug() <<"Data not good, skip" <<Qt::endl;

    }
    poredi(); // poziv funkcije koja treba da kontrolise ventilator i grejalicu
}

void Dialog::poredi() // sluzi za kontrolu i u automackom i u manuel modu rada grejalice i ventilatora
{
    pinMode( REL1, OUTPUT );
    pinMode( REL2, OUTPUT );

    if(temperature > temperature_gr && GrM_A == 1)
    {
        qDebug() <<"veca tem"<<Qt::endl;
        digitalWrite( REL1, HIGH );
    }
        else if(GrM_A == 0 && GrO_F == 1)
    {
        qDebug() <<"man uklj gr"<<Qt::endl;
        digitalWrite( REL1, HIGH );
    }
        else
    {
        digitalWrite( REL1, LOW );
    }

    if(huminity > huminity_gr && VnM_A == 1)
    {
        qDebug() <<"veca vla"<<Qt::endl;
        digitalWrite( REL2, HIGH );
    }
        else if(VnM_A == 0 && VnO_F == 1)
    {
        qDebug() <<"man uklj vn"<<Qt::endl;
        digitalWrite( REL2, HIGH );
    }
        else
    {
        digitalWrite( REL2, LOW );
    }
}

void Dialog::on_checkBox_clicked(bool checked) // Pali i gasi motor
{
    if(checked)
    {
        MOTORONOFF = 1; //motor je ukljucen
    }
    else
    {
        MOTORONOFF = 0; //motor je iskljucen
    }
}

void Dialog::on_checkBox_2_clicked(bool checked) //Menja smer okretanja motora
{
    if(checked)
    {
        SMER = 1; //u smeru kazeljke na satu se okrece
    }
    else
    {
        SMER = 0; //u smeru suprotnom od kazaljke na satu se okrece
    }
}

void Dialog::on_checkBox_3_clicked(bool checked) // bira izmedju dva rezima rada grejalice
{
    if(checked)
    {
        GrM_A = 1; //grejalica radi u automackom rezimu rada
    }
    else
    {
        GrM_A = 0;//grejalica radi u manuelnom rezimu rada
    }
}

void Dialog::on_checkBox_6_clicked(bool checked) //bira izmedju dva rezima rada ventilatora
{
    if(checked)
    {
        VnM_A = 1;//ventilator radi u automackom rezimu rada
    }
    else
    {
        VnM_A = 0;//ventilator radi u manuelnom rezimu rada
    }
}

void Dialog::on_checkBox_4_clicked(bool checked) // pali i gasi grejalicu u manuel modu
{
    if(checked)
    {
        GrO_F = 1;//grejalica je ukljucen
    }
    else
    {
        GrO_F = 0;//grejalica je ukljucen
    }
}

void Dialog::on_checkBox_5_clicked(bool checked) //pali i gasi ventilator u manuel modu
{
    if(checked)
    {
        VnO_F = 1;//ventilator je ukljucen
    }
    else
    {
        VnO_F = 0;//ventilator je ukljucen
    }
}
