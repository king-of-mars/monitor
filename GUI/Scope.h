#ifndef SCOPE_H
#define SCOPE_H

//Stl includes
#include <vector>
#include <iostream>

//Qt-includes
#include <QWidget>

using namespace std;

class Scope : public QWidget
{
    Q_OBJECT

    public:
         Scope(QWidget *parent = 0);
         void Set_Data(vector<float> data_in);

    protected:
         void paintEvent(QPaintEvent *event);

    private:
        /*!
        Returns a vector indicating the following values:
        \returns
        \return v[0]: min_x
        \return v[1]: max_x
        \return v[2]: min_y
        \return v[3]: max_y
        */
        vector<float> Find_Scale();

        //! Data in which the Scaling factors are stored
        vector<float> Scale;

        //! Data to be displayed in the widget
        vector<float> data;

        //! If debug is enabled
        int debug;
};

#endif
