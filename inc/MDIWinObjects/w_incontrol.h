//****************************************************************************
// MIT Media Lab - Biomechatronics
// Jean-Francois (Jeff) Duval
// jfduval@media.mit.edu
// 02/2016
//****************************************************************************
// MainWindow: Qt GUI Main file
//****************************************************************************

#ifndef W_INCONTROL_H
#define W_INCONTROL_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <QWidget>
#include "counter.h"
#include "flexsea_generic.h"
#include <QTimer>

//****************************************************************************
// Namespace & Class Definition:
//****************************************************************************

namespace Ui {
    class W_InControl;
}

class W_InControl : public QWidget, public Counter<W_InControl>
{
    Q_OBJECT

public:
    //Constructor & Destructor:
	explicit W_InControl(QWidget *parent = 0);
	virtual ~W_InControl();
	
    int getActiveSlave() const;

public slots:
    void init_tab_stream_in_ctrl(void);
    void stream_in_ctrl(void);
    void on_pushButton_inctrl_w0_clicked();
    void on_pushButton_inctrl_w1_clicked();
    void on_pushButton_inctrl_w2_clicked();
    void on_pushButton_inctrl_w3_clicked();
    void write_in_control_w(uint8_t var);

signals:
    void writeCommand(uint8_t numb, uint8_t *tx_data, uint8_t r_w);
    void windowClosed(void);

private:
    Ui::W_InControl *ui;
    QTimer* streamTimer;

    void updateUIData(void);


private slots:
    void streamInControl(void);

};

#endif // W_INCONTROL_H
