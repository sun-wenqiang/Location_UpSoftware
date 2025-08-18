#include "thresholdDialog.h"


thresholdDialog::thresholdDialog(QWidget * parent): QDialog(parent)
{
    setWindowTitle("设置检测算法阈值");

    QVBoxLayout *layout = new QVBoxLayout(this);

    spinBox = new QSpinBox(this);
    spinBox->setValue(1);
    layout->addWidget(spinBox);

    gainLabel = new QLabel(this);
    gainLabel->setText("");
    layout->addWidget(gainLabel);

    QPushButton *okButton = new QPushButton("确定", this);
    QPushButton *cancelButton = new QPushButton("取消", this);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(okButton);
    btnLayout->addWidget(cancelButton);
    layout->addLayout(btnLayout);

    connect(okButton, &QPushButton::clicked, this, &thresholdDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &thresholdDialog::reject);
    connect(spinBox, qOverload<int>(&QSpinBox::valueChanged), [=](int val){
        if (val>=0 && val<gainValues.size())
            gainLabel->setText(QString("对应增益：%1 dB").arg(gainValues[val]));
        else
            gainLabel->setText("");
    });
    this->resize(300, 150);
}

void thresholdDialog::setGainValues(const QVector<int> & gains)
{
    gainValues = gains;
    int val = spinBox->value();
    if (val>=0 && val<gainValues.size())
    {
        gainLabel->setText(QString("对应增益：%1 dB").arg(gainValues[val]));
    }
    else
    {
        gainLabel->setText("");
    }
}

void thresholdDialog::setRange(int min, int max) 
{
    spinBox->setRange(min, max);
}

void thresholdDialog::setDefaultValue(int value) 
{
    spinBox->setValue(value);
}

int thresholdDialog::getValue() const
{
    return spinBox->value();
}