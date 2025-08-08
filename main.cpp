#include <QApplication>
#include <QPushButton>
#include "algorithm/algorithm.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QPushButton button("Hello Qt!");
    button.show();
    return app.exec();
}
