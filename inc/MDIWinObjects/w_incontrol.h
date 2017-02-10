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
    void stream_in_ctrl(void);
	void init_tab_stream_in_ctrl(void);
    void on_pushButton_inctrl_w0_clicked();
    void on_pushButton_inctrl_w1_clicked();
    void on_pushButton_inctrl_w2_clicked();
    void on_pushButton_inctrl_w3_clicked();
    void write_in_control_w(uint8_t var);
	void updateUIData(void);

signals:
	void windowClosed(void);

private:
    Ui::W_InControl *ui;

};

#endif // W_INCONTROL_H
