#pragma once
#ifndef TABLE_HELPERS_H
#define TABLE_HELPERS_H

#include <QHeaderView>
#include <QTableWidget>
#include <QVariant>
#include <functional>
#include <memory>
#include <vector>

class SortableTableWidgetItem : public QTableWidgetItem {
public:
    explicit SortableTableWidgetItem(const QString& text, const QVariant& key = QVariant())
        : QTableWidgetItem(text) {
        setData(Qt::UserRole, key.isValid() ? key : QVariant(text));
    }

    bool operator<(const QTableWidgetItem& other) const override {
        const QVariant left = data(Qt::UserRole);
        const QVariant right = other.data(Qt::UserRole);
        const int leftType = left.metaType().id();
        const int rightType = right.metaType().id();
        const bool leftNumeric = leftType == QMetaType::Int || leftType == QMetaType::UInt ||
                                 leftType == QMetaType::LongLong || leftType == QMetaType::ULongLong ||
                                 leftType == QMetaType::Double;
        const bool rightNumeric = rightType == QMetaType::Int || rightType == QMetaType::UInt ||
                                  rightType == QMetaType::LongLong || rightType == QMetaType::ULongLong ||
                                  rightType == QMetaType::Double;
        if (leftNumeric && rightNumeric) return left.toDouble() < right.toDouble();
        return left.toString().toCaseFolded() < right.toString().toCaseFolded();
    }
};

inline QTableWidgetItem* sortableItem(const QString& text, const QVariant& key = QVariant()) {
    return new SortableTableWidgetItem(text, key);
}

inline void enableThreeStateSorting(QTableWidget* table, const std::function<void()>& restoreRows,
                                    const std::function<void(const QString&)>& reportStatus) {
    std::vector<QString> titles;
    titles.reserve(static_cast<size_t>(table->columnCount()));
    for (int column = 0; column < table->columnCount(); ++column)
        titles.push_back(table->horizontalHeaderItem(column)->text());
    auto lastColumn = std::make_shared<int>(-1);
    auto step = std::make_shared<int>(0);
    table->setSortingEnabled(false);
    QObject::connect(table->horizontalHeader(), &QHeaderView::sectionClicked, table,
                     [table, restoreRows, reportStatus, titles, lastColumn, step](int column) {
        if (*lastColumn != column) {
            *lastColumn = column;
            *step = 0;
        }
        *step = (*step + 1) % 3;
        for (int index = 0; index < table->columnCount(); ++index)
            table->horizontalHeaderItem(index)->setText(titles[static_cast<size_t>(index)]);
        if (*step == 0) {
            restoreRows();
            reportStatus("Đã khôi phục thứ tự ban đầu.");
            return;
        }
        const bool descending = *step == 1;
        table->sortItems(column, descending ? Qt::DescendingOrder : Qt::AscendingOrder);
        table->horizontalHeaderItem(column)->setText(
            titles[static_cast<size_t>(column)] + (descending ? " ↓" : " ↑"));
        reportStatus(descending ? "Sắp xếp giảm dần." : "Sắp xếp tăng dần.");
    });
}

#endif
