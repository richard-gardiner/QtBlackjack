
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sstream>
#include "dragwidget.h"
#include <QGroupBox>
#include <QMessageBox>

//using namespace Phonon;

#define MinimumBet 5
/*#define XScalingFactor 0.533
#define YScalingFactor 0.711

#define XScalingFactor 1
#define YScalingFactor 1*/

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
	QRect Screen = QApplication::desktop()->screenGeometry();

	/* Scale = User's screen size / My screen size */
	float wScale = Screen.width() / 1920.0;
	float hScale = Screen.height() / 1080.0;

	XScalingFactor = wScale;
	YScalingFactor = hScale;

	// Setup sound object
	media = new Phonon::MediaObject(this);
	createPath(media, new Phonon::AudioOutput(Phonon::MusicCategory, this));

	// Group the sets of cards to make
	// it easier to move them together
	PlayersCards = new QGroupBox(this);
	DealersCards = new QGroupBox(this);

	// Set the size and geometry of each card hand container
	PlayersCards->setGeometry(195*XScalingFactor, 310*YScalingFactor, 300*XScalingFactor, 300*YScalingFactor);
	DealersCards->setGeometry(230*XScalingFactor, 50*YScalingFactor, 300*XScalingFactor, 300*YScalingFactor);

/*	int posX = 195;
	int posXDealer =230;*/
	int posX = 0;
	int posXDealer = 0;

	// Create a card label for every possible card in each hand
	// No hand can hold more than 11 cards
	for(int CardNumber = 0; CardNumber < 11; CardNumber++)
	{
		posX += 24;
		posXDealer += 19;

		DealersHand[CardNumber] = new QLabel(DealersCards);
		DealersHand[CardNumber]->setGeometry(posXDealer*XScalingFactor, 0, 191*XScalingFactor, 250*YScalingFactor);
		DealersHand[CardNumber]->lower();

		PlayersHand[CardNumber] = new QLabel(PlayersCards);
		PlayersHand[CardNumber]->setGeometry(posX*XScalingFactor, 0, 191*XScalingFactor, 250*YScalingFactor);
		PlayersHand[CardNumber]->lower();
	}

	// Create a set of menu actions
  	NewGame = new QAction("&New Game", this);
	ToggleSound = new QAction("&Toggle Sound", this);
	About = new QAction("&About", this);
	Quit = new QAction("&Quit", this);

	// Allow ToggleSound action to be checkable
	ToggleSound->setCheckable(true);
	ToggleSound->setChecked(true);

	// Create a menu and populate it with the actions
	QMenu *Menu;
	Menu = menuBar()->addMenu("&Menu");
	Menu->addAction(NewGame);
	Menu->addAction(ToggleSound);
	Menu->addAction(About);
	Menu->addAction(Quit);

	// Link menu actions to functions
	connect(About, SIGNAL(triggered()), this, SLOT(DisplayAboutBox()));
	connect(Quit, SIGNAL(triggered()), qApp, SLOT(quit()));

	// Set the two font colours to be used throughout the game
	// Black for all text...
	QPalette* TextPalette = new QPalette();
	TextPalette->setColor(QPalette::WindowText, Qt::black);

	// ...except the bet value text which is a kind of gold
	QRgb BetTextRGBValue;
	BetTextRGBValue = qRgb(205, 205, 0);
	QPalette* BetValueTextPalette = new QPalette();
	BetValueTextPalette->setColor(QPalette::WindowText, BetTextRGBValue);

	// Setup the different fonts to be used throughout the game...
	QFont LabelFont("mry_KacstQurn");
	QFont StatusFont("mry_KacstQurn");
	QFont ResultsFont("mry_KacstQurn");
	QFont HandValueFont("mry_KacstQurn");

	// ...and their associated sizes
	LabelFont.setPointSizeF(24.0*XScalingFactor);
	StatusFont.setPointSizeF(22.0*XScalingFactor);
	ResultsFont.setPointSizeF(16.0*XScalingFactor);
	HandValueFont.setPointSizeF(16.0*XScalingFactor);

/*	QFont LabelFont("mry_KacstQurn", 18);
	QFont StatusFont("mry_KacstQurn", 14);
	QFont ResultsFont("mry_KacstQurn", 10);
	QFont HandValueFont("mry_KacstQurn", 10);*/

	labelBetRing = new QLabel(this);
//	labelBetRing->setGeometry(240*XScalingFactor, 520*YScalingFactor, 151*XScalingFactor, 111*YScalingFactor);
	labelBetRing->setGeometry(240*XScalingFactor, 520*YScalingFactor, 140*XScalingFactor, 89*YScalingFactor);
	QPixmap BetRingPixMap(":/Images/Bet_Ring2.gif");
	labelBetRing->setScaledContents(true);
	labelBetRing->setPixmap(BetRingPixMap);

	labelStackSpot = new QLabel(this);
	labelStackSpot->setGeometry(240*XScalingFactor, 665*YScalingFactor, 139*XScalingFactor, 39*YScalingFactor);
	QPixmap StackSpotPixMap(":/Images/StackSpot.png");
	labelStackSpot->setScaledContents(true);
	labelStackSpot->setPixmap(StackSpotPixMap);

	labelHandValueSpot = new QLabel(this);
	labelHandValueSpot->setGeometry(240*XScalingFactor, 655*YScalingFactor, 139*XScalingFactor, 39*YScalingFactor);
	QPixmap HandValueSpotPixMap(":/Images/HandValueSpot.png");
	labelHandValueSpot->setScaledContents(true);
	labelHandValueSpot->setPixmap(HandValueSpotPixMap);
	labelHandValueSpot->setVisible(false);

	labelDealersHandValueSpot = new QLabel(this);
	labelDealersHandValueSpot->setGeometry(240*XScalingFactor, 655*YScalingFactor, 139*XScalingFactor, 39*YScalingFactor);
	QPixmap DealerHandValueSpotPixMap(":/Images/HandValueSpot.png");
	labelDealersHandValueSpot->setScaledContents(true);
	labelDealersHandValueSpot->setPixmap(HandValueSpotPixMap);
	labelDealersHandValueSpot->setVisible(false);

	labelResultsBubble = new QLabel(this);
	labelResultsBubble->setGeometry(25*XScalingFactor, 250*YScalingFactor, 567*XScalingFactor, 165*YScalingFactor);
	QPixmap ResultsBubblePixMap(":/Images/ResultsSummary.gif");
	labelResultsBubble->setScaledContents(true);	
	labelResultsBubble->setPixmap(ResultsBubblePixMap);
	labelResultsBubble->setVisible(false);

	labelStatusBubble = new QLabel(this);
	labelStatusBubble->setGeometry(20*XScalingFactor, 15*YScalingFactor, 559*XScalingFactor, 94*YScalingFactor);
//	labelStatusBubble->setGeometry(10.66, 8, 297.97, 50);
	QPixmap StatusBubblePixMap(":/Images/StatusBubble.png");
	labelStatusBubble->setScaledContents(true);
	labelStatusBubble->setPixmap(StatusBubblePixMap);

	labelBetValue = new QLabel(this);
	labelBetValue->setGeometry(110*XScalingFactor, 555*YScalingFactor, 121*XScalingFactor, 41*YScalingFactor);
	labelBetValue->setPalette(*BetValueTextPalette);
	labelBetValue->setFont(LabelFont);
	labelBetValue->setAlignment(Qt::AlignRight);

	labelStackValue = new QLabel(this);
	labelStackValue->setGeometry(250*XScalingFactor, 662*YScalingFactor, 121*XScalingFactor, 41*YScalingFactor);
//	labelStackValue->setGeometry(133, 352.88, 64.50, 21.86);
	labelStackValue->setPalette(*TextPalette);	
	labelStackValue->setFont(LabelFont);
	labelStackValue->setAlignment(Qt::AlignCenter);

	labelGameStatus = new QLabel(this);
	labelGameStatus->setGeometry(30*XScalingFactor, 20*YScalingFactor, 541*XScalingFactor, 81*YScalingFactor);
	labelGameStatus->setPalette(*TextPalette);
	labelGameStatus->setFont(StatusFont);
	labelGameStatus->setAlignment(Qt::AlignCenter);

	labelPlayersHandValue = new QLabel(this);
	labelPlayersHandValue->setPalette(*TextPalette);
	labelPlayersHandValue->setFont(HandValueFont);
	labelPlayersHandValue->setAlignment(Qt::AlignCenter);
	labelPlayersHandValue->setVisible(false);

	labelDealersHandValue = new QLabel(this);
	labelDealersHandValue->setPalette(*TextPalette);
	labelDealersHandValue->setFont(HandValueFont);
	labelDealersHandValue->setAlignment(Qt::AlignCenter);
	labelDealersHandValue->setVisible(false);

	labelResultsSummary = new QLabel(this);
	labelResultsSummary->setGeometry(35*XScalingFactor, 270*YScalingFactor, 547*XScalingFactor, 315*YScalingFactor);
	labelResultsSummary->setPalette(*TextPalette);
	labelResultsSummary->setFont(ResultsFont);
	labelResultsSummary->setAlignment(Qt::AlignTop);
	//labelResultsSummary->raise();
	labelResultsSummary->setVisible(false);

	labelBustText = new QLabel(this);
	labelBustText->setGeometry(160*XScalingFactor, 330*YScalingFactor, 321*XScalingFactor, 141*YScalingFactor);
	QPixmap BustTextPixMap(":/Images/BustText.gif");
	labelBustText->setPixmap(BustTextPixMap);
	labelBustText->setScaledContents(true);
	labelBustText->setVisible(false);

	labelDealersBustText = new QLabel(this);
	labelDealersBustText->setGeometry(160*XScalingFactor, 120*YScalingFactor, 291*XScalingFactor, 111*YScalingFactor);
	labelDealersBustText->setPixmap(BustTextPixMap);
	labelDealersBustText->setScaledContents(true);
	labelDealersBustText->setVisible(false);

	labelBlackjackText = new QLabel(this);
	labelBlackjackText->setGeometry(100*XScalingFactor, 330*YScalingFactor, 400*XScalingFactor, 227*YScalingFactor);
	QPixmap BlackjackTextPixMap(":/Images/BlackjackText.gif");
	labelBlackjackText->setPixmap(BlackjackTextPixMap);
	labelBlackjackText->setScaledContents(true);
	labelBlackjackText->setVisible(false);

	labelDealersBlackjackText = new QLabel(this);
	labelDealersBlackjackText->setGeometry(140*XScalingFactor, 125*YScalingFactor, 320*XScalingFactor, 168*YScalingFactor);
	labelDealersBlackjackText->setPixmap(BlackjackTextPixMap);
	labelDealersBlackjackText->setScaledContents(true);
	labelDealersBlackjackText->setVisible(false);

    FivePile = new DragWidget(this, false);
	FivePile->populate(5);
	FivePile->setGeometry(15*XScalingFactor, 690*YScalingFactor, 88*XScalingFactor, 61*YScalingFactor);
	FivePile->raise();

	TenPile = new DragWidget(this, false);
	TenPile->populate(10);
	TenPile->setGeometry(135*XScalingFactor, 705*YScalingFactor, 88*XScalingFactor, 61*YScalingFactor);
	TenPile->raise();

	TwentyFivePile = new DragWidget(this, false);	
	TwentyFivePile->populate(25);
	TwentyFivePile->setGeometry(265*XScalingFactor, 715*YScalingFactor, 88*XScalingFactor, 61*YScalingFactor);
	TwentyFivePile->raise();

	FiftyPile = new DragWidget(this, false);	
	FiftyPile->populate(50);
	FiftyPile->setGeometry(385*XScalingFactor, 705*YScalingFactor, 88*XScalingFactor, 61*YScalingFactor);
	FiftyPile->raise();

	HundredPile = new DragWidget(this, false);	
	HundredPile->populate(100);
	HundredPile->setGeometry(500*XScalingFactor, 690*YScalingFactor, 88*XScalingFactor, 61*YScalingFactor);
	HundredPile->raise();

	BettingPile = new DragWidget(this, true);
	BettingPile->populate(200);
//	BettingPile->setGeometry(267, 414, 88, 200);
	BettingPile->setGeometry(210*XScalingFactor, 414*YScalingFactor, 200*XScalingFactor, 220*YScalingFactor);

	DoneButton = new ClickableLabel(this);
	DoneButton->setGeometry(300*XScalingFactor, 580*YScalingFactor, 100*XScalingFactor, 37*YScalingFactor);
	QPixmap DoneButtonPixMap(":/Images/DoneGreenButtonSmall.png");
	DoneButton->setScaledContents(true);
	DoneButton->setPixmap(DoneButtonPixMap);
	
	HitButton = new ClickableLabel(this);
	HitButton->setGeometry(335*XScalingFactor, 350*YScalingFactor, 100*XScalingFactor, 37*YScalingFactor);
	QPixmap HitButtonPixMap(":/Images/HitButtonSmall.png");
	HitButton->setScaledContents(true);
	HitButton->setPixmap(HitButtonPixMap);
	HitButton->raise();

	StandButton = new ClickableLabel(this);
	StandButton->setGeometry(345*XScalingFactor, 382*YScalingFactor, 100*XScalingFactor, 37*YScalingFactor);
	QPixmap StandButtonPixMap(":/Images/StandButtonSmall.png");
	StandButton->setScaledContents(true);
	StandButton->setPixmap(StandButtonPixMap);

	YesButton = new ClickableLabel(this);
	YesButton->setGeometry(335*XScalingFactor, 350*YScalingFactor, 100*XScalingFactor, 37*YScalingFactor);
	QPixmap YesButtonPixMap(":/Images/YesButtonSmall.png");
	YesButton->setScaledContents(true);
	YesButton->setPixmap(YesButtonPixMap);

	NoButton = new ClickableLabel(this);
	NoButton->setGeometry(345*XScalingFactor, 382*YScalingFactor, 100*XScalingFactor, 37*YScalingFactor);
	QPixmap NoButtonPixMap(":/Images/NoButtonSmall.png");
	NoButton->setScaledContents(true);
	NoButton->setPixmap(NoButtonPixMap);

	SurrenderButton = new ClickableLabel(this);
	SurrenderButton->setGeometry(355*XScalingFactor, 414*YScalingFactor, 100*XScalingFactor, 37*YScalingFactor);
	QPixmap SurrenderButtonPixMap(":/Images/SurrenderButtonSmall.png");
	SurrenderButton->setScaledContents(true);
	SurrenderButton->setPixmap(SurrenderButtonPixMap);

	SplitButton = new ClickableLabel(this);
	SplitButton->setGeometry(365*XScalingFactor, 446*YScalingFactor, 100*XScalingFactor, 37*YScalingFactor);
	QPixmap SplitButtonPixMap(":/Images/SplitButtonSmall.png");
	SplitButton->setScaledContents(true);
	SplitButton->setPixmap(SplitButtonPixMap);

	DoubleButton = new ClickableLabel(this);
	DoubleButton->setGeometry(375*XScalingFactor, 478*YScalingFactor, 100*XScalingFactor, 37*YScalingFactor);
	QPixmap DoubleButtonPixMap(":/Images/DoubleButtonSmall.png");
	DoubleButton->setScaledContents(true);
	DoubleButton->setPixmap(DoubleButtonPixMap);

//###################### About Box ##########################

	AboutBox = new QWidget;
	AboutBox->setGeometry(50, 300, 525, 155);
	AboutBox->setWindowTitle("About Blackjack");

	QPixmap AboutBoxPixMap("/home/jimbo/Dropbox/QtProjects/BlackJackNewGUI/JimboFace.gif");
	AboutBoxPixMap = AboutBoxPixMap.scaled(100, 100);

    QVBoxLayout *AboutLayout = new QVBoxLayout(AboutBox);
    QHBoxLayout *TextLayout = new QHBoxLayout();
    QHBoxLayout *ButtonLayout = new QHBoxLayout();

	labelAboutPicture = new QLabel(AboutBox);
	labelAboutPicture->setPixmap(AboutBoxPixMap);
	labelAboutPicture->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	labelAbout = new QLabel(AboutBox);
	labelAbout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	labelAbout->setWordWrap(true);
	labelAbout->setScaledContents(true);

	OKButton = new QPushButton("&OK", AboutBox);
	LicenceButton = new QPushButton("&License", AboutBox);
	CreditsButton = new QPushButton("&Credits", AboutBox);

	AboutBoxMapper = new QSignalMapper();
	connect(OKButton, SIGNAL(clicked()), AboutBox, SLOT(hide()));
	connect(LicenceButton, SIGNAL(clicked()), AboutBoxMapper, SLOT(map()));
	connect(CreditsButton, SIGNAL(clicked()), AboutBoxMapper, SLOT(map()));
	AboutBoxMapper->setMapping(LicenceButton, 2);
	AboutBoxMapper->setMapping(CreditsButton, 3);
	connect(AboutBoxMapper, SIGNAL(mapped(int)), this, SLOT(ChangeAboutBoxText(int)));

	OKButton->setDefault(true);

	TextLayout->addWidget(labelAboutPicture);
	TextLayout->addWidget(labelAbout);

	ButtonLayout->addWidget(LicenceButton);
	ButtonLayout->addWidget(CreditsButton);
	ButtonLayout->addWidget(OKButton);

	AboutLayout->addLayout(TextLayout);
	AboutLayout->addLayout(ButtonLayout);

	ChangeAboutBoxText(1);

	AboutBox->setLayout(AboutLayout);
	//AboutBox->setFixedSize(AboutBox->size());
	AboutBox->setWindowFlags(Qt::Dialog);

//###########################################################

//##### func

	BettingPile->raise();

	myThread = new GameThread();

	connect(myThread, SIGNAL(updatePlayersHand(QString,int)), this, SLOT(UpdatePlayersHand(QString,int)));
	connect(myThread, SIGNAL(updateDealersHand(QString,int)), this, SLOT(UpdateDealersHand(QString,int)));
	connect(myThread, SIGNAL(updateDealersHandValue(QString)), this, SLOT(UpdateDealersHandValue(QString)));
	connect(myThread, SIGNAL(updatePlayersHandValue(QString)), this, SLOT(UpdatePlayersHandValue(QString)));
	connect(myThread, SIGNAL(updateStack(QString)), this, SLOT(UpdateStackValue(QString)));
	connect(myThread, SIGNAL(updateBet(QString)), this, SLOT(UpdateBetValue(QString)));
	connect(myThread, SIGNAL(clearPlayersHand()), this, SLOT(ClearPlayersHand()));
	connect(myThread, SIGNAL(clearDealersHand()), this, SLOT(ClearDealersHand()));
	connect(myThread, SIGNAL(Clearchips()), this, SLOT(HideHandValueSpots()));
	connect(myThread, SIGNAL(ButtonVisibility(bool, bool, bool, bool, bool, bool, bool, bool)), this, SLOT(HideButtons(bool, bool, bool, bool, bool, bool, bool, bool)));
	connect(myThread, SIGNAL(ResultTextVisibility(bool, bool, bool, bool)), this, SLOT(ResultText(bool, bool, bool, bool)));
	connect(myThread, SIGNAL(updateStatus(QString)), this, SLOT(UpdateGameStatus(QString)));
	connect(myThread, SIGNAL(updateResultsSummary(QString)), this, SLOT(UpdateResultsSummary(QString)));
	connect(myThread, SIGNAL(DisableChips(bool)), this, SLOT(DisableChips(bool)));
	connect(myThread, SIGNAL(GameOver()), this, SLOT(PositionYesNo()));
	connect(myThread, SIGNAL(PlayWinSound()), this, SLOT(PlayWinSound()));
	connect(myThread, SIGNAL(PlayLoseSound()), this, SLOT(PlayLoseSound()));

	connect(BettingPile, SIGNAL(AddToBet(float)), myThread, SIGNAL(IncreaseBet(float)));
	connect(BettingPile, SIGNAL(RemoveFromBet(float)), myThread, SIGNAL(DecreaseBet(float)));
	connect(BettingPile, SIGNAL(PlayChipSound()), this, SLOT(PlayChipSound()));

	connect(FivePile, SIGNAL(PlayChipSound()), this, SLOT(PlayChipSound()));
	connect(TenPile, SIGNAL(PlayChipSound()), this, SLOT(PlayChipSound()));
	connect(TwentyFivePile, SIGNAL(PlayChipSound()), this, SLOT(PlayChipSound()));
	connect(FiftyPile, SIGNAL(PlayChipSound()), this, SLOT(PlayChipSound()));
	connect(HundredPile, SIGNAL(PlayChipSound()), this, SLOT(PlayChipSound()));

	connect(myThread, SIGNAL(Clearchips()), BettingPile, SLOT(ClearChips()));

	// #1

	m_sigmapper = new QSignalMapper();

	// #2

	connect(HitButton, SIGNAL(clicked()), m_sigmapper, SLOT(map()));
	connect(StandButton, SIGNAL(clicked()), m_sigmapper, SLOT(map()));
	connect(DoubleButton, SIGNAL(clicked()), m_sigmapper, SLOT(map()));
	connect(SplitButton, SIGNAL(clicked()), m_sigmapper, SLOT(map()));
	connect(SurrenderButton, SIGNAL(clicked()), m_sigmapper, SLOT(map()));
	connect(YesButton, SIGNAL(clicked()), m_sigmapper, SLOT(map()));
	connect(NoButton, SIGNAL(clicked()), m_sigmapper, SLOT(map()));

	connect(DoneButton, SIGNAL(clicked()), myThread, SIGNAL(BetDone()));
/////////////////////////
	connect(NewGame, SIGNAL(triggered()), m_sigmapper, SLOT(map()));
////////////////////////

	// #3
	m_sigmapper->setMapping(HitButton, 1);
	m_sigmapper->setMapping(StandButton, 2);
	m_sigmapper->setMapping(DoubleButton, 3);
	m_sigmapper->setMapping(SplitButton, 4);
	m_sigmapper->setMapping(SurrenderButton, 5);
	m_sigmapper->setMapping(YesButton, 6);
	m_sigmapper->setMapping(NoButton, 7);
	m_sigmapper->setMapping(NewGame, 8);

	// #4
	connect(m_sigmapper, SIGNAL(mapped(int)), myThread, SLOT(ChoiceMade(int)));
	connect(myThread, SIGNAL(finished()), this, SLOT(PositionYesNo()));
	
	MakeConnections();
}

MainWindow::~MainWindow()
{
	//delete myThread;
    delete ui;
}

void MainWindow::MakeConnections()
{
	UpdateResultsSummary("");
	UpdateStackValue("100");
	disconnect(YesButton, SIGNAL(clicked()), this, SLOT(MakeConnections()));
	disconnect(NoButton, SIGNAL(clicked()), qApp, SLOT(quit()));
	connect(YesButton, SIGNAL(clicked()), m_sigmapper, SLOT(map()));
	connect(NoButton, SIGNAL(clicked()), m_sigmapper, SLOT(map()));
	myThread->start();
}

void MainWindow::DisableChips(bool ActiveState)
{
	FivePile->setInactive(ActiveState);
	TenPile->setInactive(ActiveState);
	TwentyFivePile->setInactive(ActiveState);
	FiftyPile->setInactive(ActiveState);
	HundredPile->setInactive(ActiveState);
	BettingPile->setInactive(ActiveState);
}

void MainWindow::HideHandValueSpots()
{
	labelHandValueSpot->setVisible(false);
	labelDealersHandValueSpot->setVisible(false);
	labelPlayersHandValue->setVisible(false);
	labelDealersHandValue->setVisible(false);
}

void MainWindow::ChangeAboutBoxText(int TextSet)
{
	LicenceButton->show();
	CreditsButton->show();

	disconnect(OKButton, SIGNAL(clicked()), AboutBox, SLOT(hide()));
	connect(OKButton, SIGNAL(clicked()), AboutBoxMapper, SLOT(map()));
	AboutBoxMapper->setMapping(OKButton, 1);

	switch (TextSet)
	{
		case 1:
			AboutBoxText = "<b><i><font size = 4>Blackjack</i></b><br>Another JimboMonkey Production<br>Version 1.0<br><br>Copyright \x00A9 2014 JimboMonkey Productions";
			disconnect(OKButton, SIGNAL(clicked()), AboutBoxMapper, SLOT(map()));
			connect(OKButton, SIGNAL(clicked()), AboutBox, SLOT(hide()));
			break;
		case 2:
			AboutBoxText = "<b><i><font size = 4>License</i></b><br>Copyright 2014 James O'Hea<br><br>Blackjack is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.<br><br>Blackjack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.<br><br> You should have received a copy of the GNU General Public License along with Blackjack.  If not, see <http://www.gnu.org/licenses/>.<br><br>";
			LicenceButton->hide();
			AboutBox->adjustSize();
			break;
		case 3:
			AboutBoxText = "<b><i><font size = 4>Credits</i></b><br>Credits blurbbb<br>vlaaahh<br><br>";
			CreditsButton->hide();
			break;
	}
	labelAbout->setText(AboutBoxText);		
}

void MainWindow::PositionYesNo()
{
	if(labelGameStatus->text() == "Game Over")
	{
		disconnect(YesButton, SIGNAL(clicked()), m_sigmapper, SLOT(map()));
		disconnect(NoButton, SIGNAL(clicked()), m_sigmapper, SLOT(map()));
		connect(YesButton, SIGNAL(clicked()), this, SLOT(MakeConnections()));
		connect(NoButton, SIGNAL(clicked()), qApp, SLOT(quit()));
		YesButton->setGeometry(200*XScalingFactor, 360*YScalingFactor, 100*XScalingFactor, 37*YScalingFactor);
		NoButton->setGeometry(320*XScalingFactor, 360*YScalingFactor, 100*XScalingFactor, 37*YScalingFactor);
		YesButton->setVisible(true);
		NoButton->setVisible(true);
		YesButton->raise();
		NoButton->raise();
	}
	else
	{
		MakeConnections();
	}
}

void MainWindow::DisplayAboutBox()
{
	QMessageBox msgBox;


	QString labelText = "<b><i><font size = 4>Blackjack</i></b><br>Another JimboMonkey Production<br>Version 1.0<br><br>Copyright \x00A9 2014 JimboMonkey Productions";
	QString licenseText = "<b><i><font size = 4>License</i></b><br>License blurbbb<br>vlaaahh";

	msgBox.setWindowTitle("About Blackjack");
	//msgBox.setIconPixmap(AboutBoxPixMap);
	msgBox.setText(labelText);

	QSpacerItem* horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
	QGridLayout* layout = (QGridLayout*)msgBox.layout();
	layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

	AboutBox->show();

	//msgBox.exec();
}

void MainWindow::HideButtons(bool HitVisible, bool StandVisible, bool SurrenderVisible, bool DoubleVisible, bool SplitVisible, bool YesVisible, bool NoVisible, bool DoneVisible)
{
	HitButton->setVisible(HitVisible);
	StandButton->setVisible(StandVisible);
	SurrenderButton->setVisible(SurrenderVisible);
	DoubleButton->setVisible(DoubleVisible);
	SplitButton->setVisible(SplitVisible);
	YesButton->setVisible(YesVisible);
	NoButton->setVisible(NoVisible);
	DoneButton->setVisible(DoneVisible);
	HitButton->raise();
	StandButton->raise();
	SurrenderButton->raise();
	DoubleButton->raise();
	SplitButton->raise();
	YesButton->raise();
	NoButton->raise();
	DoneButton->raise();


//	BettingPile->raise();
}

// Display the appropriate 
void MainWindow::ResultText(bool BustVisible, bool DealerBustVisible, bool BlackjackVisible, bool DealerBlackjackVisible)
{
	labelBustText->setVisible(BustVisible);
	labelDealersBustText->setVisible(DealerBustVisible);
	labelBustText->raise();
	labelDealersBustText->raise();

	labelBlackjackText->setVisible(BlackjackVisible);
	labelDealersBlackjackText->setVisible(DealerBlackjackVisible);
	labelBlackjackText->raise();
	labelDealersBlackjackText->raise();

	// Only play if sound is enabled
	if (ToggleSound->isChecked())
	{
		if(BustVisible == true or DealerBustVisible == true)
		{
			media->setCurrentSource(QUrl("/home/jimbo/Dropbox/QtProjects/BlackJackNewGUI/Sounds/Bust.mp3"));
		}
		else if(BlackjackVisible == true)
		{
			media->setCurrentSource(QUrl("/home/jimbo/Dropbox/QtProjects/BlackJackNewGUI/Sounds/Yeah.mp3"));
		}
		else if(DealerBlackjackVisible == true)
		{
			media->setCurrentSource(QUrl("/home/jimbo/Dropbox/QtProjects/BlackJackNewGUI/Sounds/Gasp.mp3"));
		}
		media->play();
	}
}

// Add a card to the player's hand.  Adjust the hand position to keep it central
void MainWindow::UpdatePlayersHand(QString LoadCardName, int CardPosition)
{
	QImage CardImage;
	int NewHandPosition;

	CardImage.load(LoadCardName);

	// Only play the draw card sound if the card is drawn during play
	// ie not during the initial draw
	if(CardPosition > 1)
	{
		media->setCurrentSource(QUrl("/home/jimbo/Dropbox/QtProjects/BlackJackNewGUI/Sounds/DrawCard.mp3"));
		// Only play if sound is enabled
		if (ToggleSound->isChecked())
		{
			media->play();
		}
	}

	// Load the image for the given card into the given position (and scale if needed)
	// New cards are raised to ensure all cards of hand are visible
	PlayersHand[CardPosition]->setPixmap(QPixmap::fromImage(CardImage).scaledToWidth(161*XScalingFactor));
	PlayersHand[CardPosition]->show();
	PlayersHand[CardPosition]->raise();
	
	// Adjust the overall hand position and value as cards are drawn to ensure the hand remains centered on the screen
	NewHandPosition = 216 - ((CardPosition + 1) * 12);
	PlayersCards->setGeometry(NewHandPosition*XScalingFactor, 310*YScalingFactor, 541*XScalingFactor, 331*YScalingFactor);
	labelHandValueSpot->setGeometry((238 - (CardPosition + 1) * 12)*XScalingFactor, 470*YScalingFactor, 55*XScalingFactor, 55*YScalingFactor);
	labelPlayersHandValue->setGeometry((210 - (CardPosition + 1) * 12)*XScalingFactor, 482*YScalingFactor, 111*XScalingFactor, 31*YScalingFactor);
	labelHandValueSpot->raise();
	labelPlayersHandValue->raise();

	// Adjust all the button positions as well
	HitButton->setGeometry((315 + (CardPosition + 1) * 10)*XScalingFactor, 350*YScalingFactor, 100*XScalingFactor, 37*YScalingFactor);
	YesButton->setGeometry((315 + (CardPosition + 1) * 10)*XScalingFactor, 350*YScalingFactor, 100*XScalingFactor, 37*YScalingFactor);
	StandButton->setGeometry((325 + (CardPosition + 1) * 10)*XScalingFactor, 382*YScalingFactor, 100*XScalingFactor, 37*YScalingFactor);
	NoButton->setGeometry((325 + (CardPosition + 1) * 10)*XScalingFactor, 382*YScalingFactor, 100*XScalingFactor, 37*YScalingFactor);
	SurrenderButton->setGeometry((335 + (CardPosition + 1) * 10)*XScalingFactor, 414*YScalingFactor, 100*XScalingFactor, 37*YScalingFactor);
	SplitButton->setGeometry((345 + (CardPosition + 1) * 10)*XScalingFactor, 446*YScalingFactor, 100*XScalingFactor, 37*YScalingFactor);
	DoubleButton->setGeometry((355 + (CardPosition + 1) * 10)*XScalingFactor, 478*YScalingFactor, 100*XScalingFactor, 37*YScalingFactor);
}

// Add a card to the dealer's hand.  Adjust the hand position to keep it central
void MainWindow::UpdateDealersHand(QString LoadCardName, int CardPosition)
{
	QImage CardImage;
	int NewHandPosition;

	CardImage.load(LoadCardName);	

	// Only play the draw card sound if the card is drawn during play
	// ie not during the initial draw
	if(CardPosition > 0 and LoadCardName != "DealerCards/CardBack.png")
	{
		media->setCurrentSource(QUrl("/home/jimbo/Dropbox/QtProjects/BlackJackNewGUI/Sounds/DrawCard.mp3"));
		// Only play if sound is enabled
		if (ToggleSound->isChecked())
		{
			media->play();
		}
	}

	// Load the image for the given card into the given position (and scale if needed)
	// New cards are raised to ensure all cards of hand are visible
	DealersHand[CardPosition]->setPixmap(QPixmap::fromImage(CardImage).scaledToWidth(94*XScalingFactor));
	DealersHand[CardPosition]->show();
	DealersHand[CardPosition]->raise();

	// Adjust the overall hand position and value as cards are drawn to ensure the hand remains centered on the screen
	NewHandPosition = 250 - ((CardPosition + 1) * 10);
	DealersCards->setGeometry(NewHandPosition*XScalingFactor, 50*YScalingFactor, 541*XScalingFactor, 331*YScalingFactor);
	labelDealersHandValueSpot->setGeometry((265 - (CardPosition + 1) * 10)*XScalingFactor, 170*YScalingFactor, 55*XScalingFactor, 55*YScalingFactor);
	labelDealersHandValue->setGeometry((235 - (CardPosition + 1) * 10)*XScalingFactor, 181*YScalingFactor, 111*XScalingFactor, 31*YScalingFactor);
	labelDealersHandValueSpot->raise();
	labelDealersHandValue->raise();
}

// Play a win sound
void MainWindow::PlayWinSound()
{
	// Only play if sound is enabled
	if (ToggleSound->isChecked())
	{
		media->setCurrentSource(QUrl("/home/jimbo/Dropbox/QtProjects/BlackJackNewGUI/Sounds/Ching.mp3"));
		media->play();
	}
}

// Play a fail sound
void MainWindow::PlayLoseSound()
{
	// Only play if sound is enabled
	if (ToggleSound->isChecked())
	{
		media->setCurrentSource(QUrl("/home/jimbo/Dropbox/QtProjects/BlackJackNewGUI/Sounds/Punch.mp3"));
		media->play();
	}
}

// Play a random chip movement sound
void MainWindow::PlayChipSound()
{
	int RandomNumber;

	// Only play if sound is enabled
	if (ToggleSound->isChecked())
	{
		// Initialize random seed from time
		srand (time(NULL));

		// Generate a random number between 1 and 3
		RandomNumber = ((rand() % 3) + 1);

		// Pick one of 3 sounds
		switch (RandomNumber)
		{
			case 1:
				media->setCurrentSource(QUrl("/home/jimbo/Dropbox/QtProjects/BlackJackNewGUI/Sounds/Chip.mp3"));
				break;
			case 2:
				media->setCurrentSource(QUrl("/home/jimbo/Dropbox/QtProjects/BlackJackNewGUI/Sounds/Chip2.mp3"));
				break;
			case 3:
				media->setCurrentSource(QUrl("/home/jimbo/Dropbox/QtProjects/BlackJackNewGUI/Sounds/Chip3.mp3"));
				break;
		}
		// Play the selected sound
		media->play();
	}	
}

// Update the game status
void MainWindow::UpdateGameStatus(QString Status)
{
	labelGameStatus->setText(Status);
}

// Update the results summary
void MainWindow::UpdateResultsSummary(QString ResultsSummary)
{
	// If a blank string is passed in, hide the summary and graphic
	if (ResultsSummary == "")
	{
		labelResultsSummary->setVisible(false);
		labelResultsBubble->setVisible(false);
	}
	else
	{
		labelResultsSummary->setVisible(true);
		labelResultsBubble->setVisible(true);
		labelResultsBubble->raise();
		labelResultsSummary->raise();
		labelResultsSummary->setText(ResultsSummary);
		labelResultsSummary->setAlignment(Qt::AlignHCenter);
	}
}

// Update the GUI player's hand value
void MainWindow::UpdatePlayersHandValue(QString HandValue)
{
	labelPlayersHandValue->setText(HandValue);
	labelHandValueSpot->setVisible(true);
	labelPlayersHandValue->setVisible(true);
}

// Update the GUI dealer's hand value
void MainWindow::UpdateDealersHandValue(QString HandValue)
{
	labelDealersHandValue->setText(HandValue);
	labelDealersHandValueSpot->setVisible(true);
	labelDealersHandValue->setVisible(true);
}

// Update stack value and refresh GUI to only present playable chips
void MainWindow::UpdateStackValue(QString StackValue)
{
	labelStackValue->setText(StackValue);
	// Float is used instead of int because half values 
	// are possible with side bet or blackjack payouts
	float NewStackValue = StackValue.toFloat();

	if(NewStackValue >= 5.0)
	{
		FivePile->setVisible(true);
	}
	else
	{
		FivePile->setVisible(false);
	}

	if(NewStackValue >= 10.0)
	{
		TenPile->setVisible(true);
	}
	else
	{
		TenPile->setVisible(false);
	}

	if(NewStackValue >= 25.0)
	{
		TwentyFivePile->setVisible(true);
	}
	else
	{
		TwentyFivePile->setVisible(false);
	}

	if(NewStackValue >= 50.0)
	{
		FiftyPile->setVisible(true);
	}
	else
	{
		FiftyPile->setVisible(false);
	}

	if(NewStackValue >= 100.0)
	{
		HundredPile->setVisible(true);
	}	
	else
	{
		HundredPile->setVisible(false);
	}
}

// Update the GUI bet value box
void MainWindow::UpdateBetValue(QString BetValue)
{
	// Don't display the Done button or
	// any number if the value is zero
	if(BetValue == "0")
	{
		BetValue = "";
		DoneButton->setVisible(false);
	}
	else
	{
		DoneButton->setVisible(true);
	}
	// Write the value to the box
	labelBetValue->setText(BetValue);
}

// 'Clear' player's hand by hiding all card labels
void MainWindow::ClearPlayersHand()
{
	int CardNumber;

	for(CardNumber = 0; CardNumber < 11; CardNumber++)
	{
		PlayersHand[CardNumber]->hide();
	}
}

// 'Clear' dealer's hand by hiding all card labels
void MainWindow::ClearDealersHand()
{
	int CardNumber;

	for(CardNumber = 0; CardNumber < 11; CardNumber++)
	{
		DealersHand[CardNumber]->hide();
	}
}

