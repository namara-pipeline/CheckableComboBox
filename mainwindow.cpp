#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "CheckableComboBox.h"

#include <QTableWidget>
#include <QPushButton>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Qt Designerで作った表の3列目各セルに、複数選択対応のコンボボックスを設定する
    QTableWidget *t = ui->tableWidget;
    int column = 2; // 3列目に設定する
    for (int row = 0; row < t->rowCount(); row++) {
        CheckableComboBox *comboBox = new CheckableComboBox();
        t->setCellWidget(row, column, comboBox);

        // コンボボックスが閉じる時の処理
        connect(comboBox, &CheckableComboBox::closedPopup, this, [=]() {
            // 現在チェックされているインデックスのリストを取得
            QStringList indexes = comboBox->getCheckedIndexList();
            // 現在チェックされている項目名のリストを取得
            QStringList textList = comboBox->getCheckedTextList();
            qDebug() << Q_FUNC_INFO << "row:" << row << "column:" << column << indexes << textList;
        });
    }

    // デバッグボタンを押したら現在の選択状態を表示して選択を全解除する
    QPushButton* button = new QPushButton("デバッグボタン", this);
    button->show(); // ボタンを表示
    // デバッグボタンを押した時の処理
    connect(button, &QPushButton::released, this, [=]() {
        // Qt Designerで作った表の3列目に複数選択対応のコンボボックスを設定したから
        QTableWidget *t = ui->tableWidget;
        int column = 2; // 3列目
        for (int row = 0; row < t->rowCount(); row++) {
            QWidget *w = t->cellWidget(row, column);
            if (w) {
                CheckableComboBox *comboBox = qobject_cast<CheckableComboBox *>( w );
                if (comboBox) {
                    // 現在の選択状態を取得
                    QStringList textList = comboBox->getCheckedTextList();
                    qDebug() << "[DEBUG]" << "row:" << row << textList;

                    // 変更可/不可をトグルさせてみる
                    bool enabled= comboBox->isEnabled();
                    comboBox->setEnabled(!enabled);

                    if (enabled) {
                        // 選択を全解除する
                        QStringList indexes = {"0"};
                        comboBox->setCheckedIndexList(indexes);
                        comboBox->setCurrentIndex(indexes.first().toInt());
                    }
                }
            }
        }
        qDebug(); // 空行
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
