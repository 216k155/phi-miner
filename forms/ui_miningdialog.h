#ifndef UI_MININGDIALOG_H
#define UI_MININGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MiningDialog
{
public:
    QWidget *MiningDialogWidget;
    QVBoxLayout *verticalLayout;
    QVBoxLayout *mainLayout;
    QGridLayout *miningInputGrid;
    QLabel *poolLabel;
    QComboBox *poolComboBox;
    QLabel *walletLabel;
    QLineEdit *walletInput;
    QHBoxLayout *miningInfoGrid;
    QVBoxLayout *verticalLayout_2;
    QListView *gpuListView;
    QLabel *outputLabel;
    QTextBrowser *textBrowser;
    QVBoxLayout *controlButtonsLayout;
    QPushButton *benchmarkButton;
    QPushButton *settingsButton;
    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *helpButton;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;

    void setupUi(QMainWindow *MiningDialog)
    {
        if (MiningDialog->objectName().isEmpty())
            MiningDialog->setObjectName(QStringLiteral("MiningDialog"));
        MiningDialog->resize(848, 668);
        MiningDialogWidget = new QWidget(MiningDialog);
        MiningDialogWidget->setObjectName(QStringLiteral("MiningDialogWidget"));
        verticalLayout = new QVBoxLayout(MiningDialogWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        mainLayout = new QVBoxLayout();
        mainLayout->setObjectName(QStringLiteral("mainLayout"));
        miningInputGrid = new QGridLayout();
        miningInputGrid->setObjectName(QStringLiteral("miningInputGrid"));
        poolLabel = new QLabel(MiningDialogWidget);
        poolLabel->setObjectName(QStringLiteral("poolLabel"));

        miningInputGrid->addWidget(poolLabel, 0, 0, 1, 1);

        poolComboBox = new QComboBox(MiningDialogWidget);
        poolComboBox->setObjectName(QStringLiteral("poolComboBox"));

        miningInputGrid->addWidget(poolComboBox, 0, 1, 1, 1);

        walletLabel = new QLabel(MiningDialogWidget);
        walletLabel->setObjectName(QStringLiteral("walletLabel"));

        miningInputGrid->addWidget(walletLabel, 2, 0, 1, 1);

        walletInput = new QLineEdit(MiningDialogWidget);
        walletInput->setObjectName(QStringLiteral("walletInput"));

        miningInputGrid->addWidget(walletInput, 2, 1, 1, 1);


        mainLayout->addLayout(miningInputGrid);

        miningInfoGrid = new QHBoxLayout();
        miningInfoGrid->setObjectName(QStringLiteral("miningInfoGrid"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        gpuListView = new QListView(MiningDialogWidget);
        gpuListView->setObjectName(QStringLiteral("gpuListView"));

        verticalLayout_2->addWidget(gpuListView);

        outputLabel = new QLabel(MiningDialogWidget);
        outputLabel->setObjectName(QStringLiteral("outputLabel"));

        verticalLayout_2->addWidget(outputLabel);

        textBrowser = new QTextBrowser(MiningDialogWidget);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));

        verticalLayout_2->addWidget(textBrowser);


        miningInfoGrid->addLayout(verticalLayout_2);

        controlButtonsLayout = new QVBoxLayout();
        controlButtonsLayout->setSpacing(0);
        controlButtonsLayout->setObjectName(QStringLiteral("controlButtonsLayout"));
        controlButtonsLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        benchmarkButton = new QPushButton(MiningDialogWidget);
        benchmarkButton->setObjectName(QStringLiteral("benchmarkButton"));

        controlButtonsLayout->addWidget(benchmarkButton);

        settingsButton = new QPushButton(MiningDialogWidget);
        settingsButton->setObjectName(QStringLiteral("settingsButton"));

        controlButtonsLayout->addWidget(settingsButton);

        startButton = new QPushButton(MiningDialogWidget);
        startButton->setObjectName(QStringLiteral("startButton"));

        controlButtonsLayout->addWidget(startButton);

        stopButton = new QPushButton(MiningDialogWidget);
        stopButton->setObjectName(QStringLiteral("stopButton"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(stopButton->sizePolicy().hasHeightForWidth());
        stopButton->setSizePolicy(sizePolicy);

        controlButtonsLayout->addWidget(stopButton);

        helpButton = new QPushButton(MiningDialogWidget);
        helpButton->setObjectName(QStringLiteral("helpButton"));
        sizePolicy.setHeightForWidth(helpButton->sizePolicy().hasHeightForWidth());
        helpButton->setSizePolicy(sizePolicy);

        controlButtonsLayout->addWidget(helpButton);

        verticalSpacer = new QSpacerItem(20, 400, QSizePolicy::Minimum, QSizePolicy::Minimum);

        controlButtonsLayout->addItem(verticalSpacer);


        miningInfoGrid->addLayout(controlButtonsLayout);


        mainLayout->addLayout(miningInfoGrid);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        pushButton = new QPushButton(MiningDialogWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout_4->addWidget(pushButton);


        mainLayout->addLayout(horizontalLayout_4);


        verticalLayout->addLayout(mainLayout);

        MiningDialog->setCentralWidget(MiningDialogWidget);

        retranslateUi(MiningDialog);

        QMetaObject::connectSlotsByName(MiningDialog);
    } // setupUi

    void retranslateUi(QMainWindow *MiningDialog)
    {
        MiningDialog->setWindowTitle(QApplication::translate("MiningDialog", "MainWindow", 0));
        MiningDialogWidget->setWindowTitle(QApplication::translate("MiningDialog", "Mining details", 0));
        poolLabel->setText(QApplication::translate("MiningDialog", "Pool", 0));
        walletLabel->setText(QApplication::translate("MiningDialog", "Wallet Address", 0));
        outputLabel->setText(QApplication::translate("MiningDialog", "Output:", 0));
        benchmarkButton->setText(QApplication::translate("MiningDialog", "Benchmark", 0));
        settingsButton->setText(QApplication::translate("MiningDialog", "Settings", 0));
        startButton->setText(QApplication::translate("MiningDialog", "Start", 0));
        stopButton->setText(QApplication::translate("MiningDialog", "Stop", 0));
        helpButton->setText(QApplication::translate("MiningDialog", "Help", 0));
        pushButton->setText(QApplication::translate("MiningDialog", "Exit", 0));
    } // retranslateUi

};

namespace Ui {
    class MiningDialog: public Ui_MiningDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MININGDIALOG_H

