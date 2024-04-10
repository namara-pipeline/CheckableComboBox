#ifndef CHECKABLECOMBOBOX_H
#define CHECKABLECOMBOBOX_H

#include <QMainWindow>
#include <QtWidgets/QStyledItemDelegate>
#include <QStandardItemModel>
#include <QComboBox>
#include <QListView>
#include <QTimer>

class CheckableComboBox : public QComboBox
{
    Q_OBJECT

public:
    CheckableComboBox(QWidget* parent = nullptr) : QComboBox(parent)
    {
        QStandardItemModel *stdModel = new QStandardItemModel(this);
        QStringList items = { "-", tr("Apple"), tr("Banana"), tr("Orane"), tr("Peach") };
        foreach (QString text, items) {
            QStandardItem *item = new QStandardItem(text);
            item->setText(text);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState((text == "-") ? Qt::Checked : Qt::Unchecked);
            stdModel->appendRow(item);
        }
        setModel(stdModel);
        setStyleSheet("background: white; border:none; padding: 2px 2px 2px 2px;");  // padding:上右下左

        setView(new QListView(this));
        view()->setItemDelegate(new QStyledItemDelegate(this));
        connect(view(), &QListView::pressed, this, [&](const QModelIndex& index) {
            QStandardItemModel *stdModel = qobject_cast<QStandardItemModel*>(model());
            QStandardItem *item = stdModel->itemFromIndex(index);
            // 何かが選択されたときの処理
            // 例："-"以外のチェックをすべて外す、1つ以上チェックがあれば"-"の項目チェックを外す
            if (item->row() == 0) { // "-"
                stdModel->item(0)->setCheckState(Qt::Checked);
                // "-"以外のチェックをすべて外す
                for (int i = 1; i < stdModel->rowCount(); i++) {
                    QStandardItem * item = stdModel->item(i);
                    item->setCheckState(Qt::Unchecked);
                }
            }
            else {  // "-"以外
                if(item->checkState() == Qt::Checked) {
                    item->setCheckState(Qt::Unchecked);
                }
                else {
                    item->setCheckState(Qt::Checked);
                }
                // 1つ以上チェックがあれば"-"の項目チェックを外す
                Qt::CheckState state = Qt::Checked;
                for (int i = 0; i < stdModel->rowCount(); i++) {
                    QStandardItem * item = stdModel->item(i);
                    if (item->checkState() == Qt::Checked) {
                        state = Qt::Unchecked;
                    }
                    stdModel->item(0)->setCheckState(state);
                }
            }
        });
    }
    void setCurrentIndex(int index) {
        if (index >= model()->rowCount()) {
            index = model()->rowCount() -1;
        }
        QComboBox::setCurrentIndex(index);
    }
    QStringList getCheckedTextList() const {
        QStringList textList;
        QStandardItemModel *stdModel = qobject_cast<QStandardItemModel*>(model());
        for (int i = 0; i < stdModel->rowCount(); i++) {
            QStandardItem* item= stdModel->item(i);
            if (item->checkState() == Qt::Checked) {
                textList.append(item->text());
            }
        }
        if (textList.count() == 0) {
            textList = {"0"};
        }
        return textList;
    }
    QStringList getCheckedIndexList() const {
        QStringList checkedList;
        QStandardItemModel *stdModel = qobject_cast<QStandardItemModel*>(model());
        for (int i = 0; i < stdModel->rowCount(); i++) {
            QStandardItem* item = stdModel->item(i);
            if (item->checkState() == Qt::Checked) {
                checkedList.append(QString::number(i));
            }
        }
        if (checkedList.count() == 0) {
            checkedList = {"0"};
        }
        return checkedList;
    }
    void setCheckedIndexList(QStringList &list) {
        QStandardItemModel *stdModel = qobject_cast<QStandardItemModel*>(model());
        for (int i = 0; i < stdModel->rowCount(); i++) {
            QStandardItem* item = stdModel->item(i);
            if (list.contains(QString::number(i))) {
                item->setCheckState(Qt::Checked);
            }
            else {
                item->setCheckState(Qt::Unchecked);
            }
        }
    }
    void setEnabled(bool enabled) { // false:コンボボックス変更不可, true:変更可
        QComboBox::setEnabled(enabled);
    }
    bool isEnabled() {
        return QComboBox::isEnabled();
    }

signals:
    void closedPopup();

private slots:
    void hidePopup() override {
        bool enabled = QComboBox::isEnabled();
        if (!enabled) {
            // enabledがtrueからfalseになる時、無駄にイベント発生するので除外する
            return;
        }
        // コンボボックスが閉じる時の通知
        emit closedPopup();
        // コンボボックスを閉じる
        QComboBox::hidePopup();
        // コンボボックスが閉じた後の処理
        QTimer::singleShot(0, this, [=]() {
            // チェックされているアイテムのうち、最も下のアイテムを表示して閉じる
            QStringList indexes = getCheckedIndexList();
            setCurrentIndex(indexes.last().toInt());
        });
    }
};
#endif // CHECKABLECOMBOBOX_H
