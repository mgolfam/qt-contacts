#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    void delListUI();

    void loadListUI();

    void flushInMemoryContacts();

    void refreshUi();

private slots:

    void on_actionAdd_triggered();

    void on_actionEdit_triggered();

    void on_actionExport_triggered();

    void on_actionSave_triggered();

    void on_actionLoad_triggered();

    void on_actionRemove_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
