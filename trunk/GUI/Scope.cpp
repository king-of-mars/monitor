#include <QtGui>

#include "Scope.h"

Scope::Scope(QWidget *parent)
 : QWidget(parent)
{
    debug = 0;

    //Update the screen
    //QTimer *timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    //timer->start(1000);

    setWindowTitle(tr("Scope"));
    resize(600, 200);
}

void Scope::Set_Data(vector<float> data_in)
{
    data.assign(data_in.begin(), data_in.end());
    Scale = Find_Scale();
    //repaint();
}

float abs(float Fin)
{
    if (Fin < 0)
        return -Fin;
    else
        return Fin;
}

vector<float> Scope::Find_Scale()
{
    vector<float> Scale(4);

    if ( data.size() == 0)
    {
        //cout<<"WARNING: No data to scale in Scope::Find_Scale()."<<endl;
        data.push_back(0);
    }

    //assert(data.size()>0);

    float max_y = data[0];
    float min_y = data[0];

    for (unsigned int x=0; x < data.size(); x++)
    {
        if ( (data[x]) > (max_y))
            max_y = (data[x]);

        if ( (data[x]) < (min_y))
            min_y = (data[x]);
    }

    Scale[0] = 0;
    Scale[1] = data.size()-1;
    Scale[2] = min_y;
    Scale[3] = max_y;

    //Avoid unpleasant states
    if (Scale[0] == Scale[1])
    {
        //cout<<"WARNING: Problem setting the scale_x."<<endl;
        Scale[0] = 0.0;
        Scale[1] = 1.0;
    }

    if (Scale[1] == Scale[2])
    {
        //cout<<"WARNING: Problem setting the scale_y."<<endl;
        Scale[2] = 0.0;
        Scale[3] = 1.0;
    }

    return Scale;
}

void Scope::paintEvent(QPaintEvent *)
{
    //Set the painting device
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    //painter.save();

    //Draws a line around the scope
    painter.setPen(Qt::black);
    painter.translate(0, 0);
    painter.drawLine(0,0 , width(),0);
    painter.drawLine(width(),0 , width(), height());
    painter.drawLine(width(), height() , 0, height());
    painter.drawLine(0, height() , 0,0);

    if (data.size() > 1)
    {
        //painter.restore();

        //Scale so that the data is spread on the entire area of the widget
        float x_denom = abs(Scale[0]) + abs(Scale[1]);
        float y_denom = abs(Scale[2]) + abs(Scale[3]);

        //assert(x_denom!=0 && y_denom!=0);

        if(debug)
        {
            cout<<x_denom<<endl;
            cout<<y_denom<<endl;
            cout<<"-"<<endl;
            cout<<(Scale[0])<<endl;
            cout<<(Scale[1])<<endl;
            cout<<(Scale[2])<<endl;
            cout<<(Scale[3])<<endl;
            cout<<"---"<<endl;
            cout<<abs(Scale[0])<<endl;
            cout<<abs(Scale[1])<<endl;
            cout<<abs(Scale[2])<<endl;
            cout<<abs(Scale[3])<<endl;
            cout<<"+"<<endl;
        }

        float scale_x = width()/(x_denom);
        float scale_y = height()/(y_denom);

        if(debug)
        {
            cout<<"-=-"<<endl;
            cout<<scale_x<<endl;
            cout<<scale_y<<endl;
        }

        //Scale the widget
        //painter.scale( scale_x, scale_y );

        float distanca_from_x_axis = 0;

        if ( Scale[2] < 0)
            distanca_from_x_axis = Scale[2];

        painter.translate( 0, height() + distanca_from_x_axis*scale_y );

        //Draws the data
        painter.setPen(Qt::blue);
        for (unsigned int x= 1; x<data.size(); x++)
        {
            //Data_y is reversed to fit an intuitive coordinate system
            painter.drawLine( int((x-1)*scale_x), int(-data[x-1]*scale_y), int(x*scale_x), int(-data[x]*scale_y) );
        }

    }//If data...
}
