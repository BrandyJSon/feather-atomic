// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020, The Monero Project.

#include "utils/utils.h"
#include "wizard/walletwizard.h"
#include "wizard/menu.h"
#include "wizard/openwallet.h"
#include "wizard/createwallet.h"
#include "wizard/network.h"
#include "wizard/createwalletseed.h"
#include "wizard/restorewallet.h"
#include "wizard/viewonlywallet.h"

#include <QStyle>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QScreen>
#include <QApplication>

WalletWizard::WalletWizard(AppContext *ctx, WalletWizard::Page startPage, QWidget *parent) : QWizard(parent),
        m_ctx(ctx) {
    this->setWindowTitle("Welcome to feather");
    this->setWindowIcon(QIcon(":/assets/images/appicons/64x64.png"));
    auto openWalletPage = new OpenWalletPage(m_ctx, this);
    auto createWallet = new CreateWalletPage(m_ctx, this);
    auto createWalletSeed = new CreateWalletSeedPage(m_ctx, this);
    setPage(Page_Menu, new MenuPage(m_ctx, this));
    setPage(Page_CreateWallet, createWallet);
    setPage(Page_OpenWallet, openWalletPage);
    setPage(Page_CreateWalletSeed, createWalletSeed);
    setPage(Page_Network, new NetworkPage(m_ctx, this));
    setPage(Page_Restore, new RestorePage(m_ctx, this));
    setPage(Page_ViewOnly, new ViewOnlyPage(m_ctx, this));

    if(config()->get(Config::firstRun).toUInt())
        setStartId(Page_Network);
    else
        setStartId(Page_Menu);

    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/assets/images/photos/1.png"));
    setWizardStyle(WizardStyle::ModernStyle);
    setOption(QWizard::NoBackButtonOnStartPage);

    connect(this, &QWizard::rejected, [=]{
        return QApplication::exit(1);
    });

    connect(createWalletSeed, &CreateWalletSeedPage::createWallet, this, &WalletWizard::createWallet);
    connect(createWallet, &CreateWalletPage::createWallet, this, &WalletWizard::createWallet);

    connect(openWalletPage, &OpenWalletPage::openWallet, [=](const QString &path){
        const auto walletPasswd = this->field("walletPasswd").toString();
        emit openWallet(path, walletPasswd);
    });
}

void WalletWizard::createWallet() {
    auto mnemonicRestoredSeed = this->field("mnemonicRestoredSeed").toString();
    auto mnemonicSeed = mnemonicRestoredSeed.isEmpty() ? this->field("mnemonicSeed").toString() : mnemonicRestoredSeed;
    const auto walletPath = this->field("walletPath").toString();
    const auto walletPasswd = this->field("walletPasswd").toString();
    auto restoreHeight = this->field("restoreHeight").toUInt();
    auto viewKey = this->field("viewOnlyViewKey").toString();
    auto spendKey = this->field("viewOnlySpendKey").toString();
    auto viewAddress = this->field("viewOnlyAddress").toString();

    if(!viewKey.isEmpty() && !viewAddress.isEmpty()) {
        auto viewHeight = this->field("viewOnlyHeight").toUInt();
        m_ctx->createWalletViewOnly(walletPath,
                                    walletPasswd,
                                    viewAddress,
                                    viewKey, spendKey, viewHeight);
        return;
    }

    auto seed = FeatherSeed::fromSeed(m_ctx->restoreHeights[m_ctx->networkType], m_ctx->coinName.toStdString(), m_ctx->seedLanguage, mnemonicSeed.toStdString());
    if(restoreHeight > 0)
        seed.setRestoreHeight(restoreHeight);
    m_ctx->createWallet(seed, walletPath, walletPasswd);
}
