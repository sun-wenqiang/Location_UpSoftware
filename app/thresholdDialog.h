#ifndef THRESHOLDDIALOG_H
#define THRESHOLDDIALOG_H
#include <QDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QVector>

class thresholdDialog: public QDialog
{
    Q_OBJECT
public:
    explicit thresholdDialog(QWidget * parent = nullptr);
    void setRange(int min, int max);
    void setDefaultValue(int value);
    int getValue() const;
    void setGainValues(const QVector<int> & gains);

private:
    QSpinBox * spinBox;
    QLabel   * gainLabel;
    QVector<int> gainValues;
};

#endif