#include <array>
#include <SFML/Graphics.hpp>

const int OFFSET(40);
const int FONT_SIZE (40);
const sf::Vector2u WINDOWS_SIZE(640, 480);

const int winCombinationList[8][3]	= {
	{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, 
	{0, 3, 6}, {1, 4, 7}, {2, 5, 8}, 
	{0, 4, 8}, {2, 4, 6}
};

class Game
{
public:
	Game();
	void run();

private:
	enum EPlayer {
		Empty, 
		Draw,
		PlayerX,
		PlayerO	
	};

private:
	void update();
	void proccessInput();
	void render();

	void drawLines(std::size_t lineIndex, int count, float space, bool isVertical);
	int findCellByClick(float cellSize, int offset, sf::Vector2i mouseButtonPosition);
	EPlayer checkVictory(const std::array<EPlayer, 9> &field);
	bool isClickInZone(const sf::Event &event, const sf::FloatRect &zone);

	void humanMove();
	void computerMove();

private:
	sf::RenderWindow						mWindow;
	sf::RectangleShape						mInfoBackground;
	sf::RectangleShape						mBattleField;
	std::array<sf::RectangleShape, 4>		mGridLines;
	float									mSpaceBetweenThinLines;
	float									mSpaceBetweenMainLines;

	bool									mIsCellSelected;
	sf::Vector2u							mSelectedCell;
	sf::RectangleShape						mSelectedCellHightligh;

	sf::Font								mFont;
	std::array<sf::Text, 4>					mMarks;
	std::array<std::array<EPlayer, 9>, 9>	mGameField;
	std::array<EPlayer, 9>					mWonCell;
	bool									mIsTurnX;
	bool									mIsPlayerReadyToMove;
	bool									mIsGameOver;
	bool									mIsGameWithComputer;

	sf::Text								mChangePlayer;
	sf::Text								mStartNewGame;
	sf::Text								mGameOverMessage;
	sf::Vector2u							mScore;
	std::array<sf::Text, 2>					mScoreText;
	std::array<sf::Text, 2>					mTurnText;
};

Game::Game() :
	mWindow(sf::VideoMode(WINDOWS_SIZE.x, WINDOWS_SIZE.y), "Tic Tac Toe ^2",
			sf::Style::Titlebar | sf::Style::Close),
	mInfoBackground(sf::Vector2f(static_cast<float>(WINDOWS_SIZE.x), 70)),
	mBattleField(),
	mGridLines(),
	mIsCellSelected(false),
	mSelectedCell(),
	mSelectedCellHightligh(),
	mFont(),
	mGameField(),
	mWonCell(),
	mIsTurnX(true),
	mIsPlayerReadyToMove(false),
	mIsGameOver(false),
	mIsGameWithComputer(true),
	mChangePlayer(),
	mStartNewGame(),
	mGameOverMessage(),
	mScore()
{
	mWindow.setFramerateLimit(60);

	mInfoBackground.setFillColor(sf::Color(240, 240, 240));
	mInfoBackground.setOutlineThickness(1.f);
	mInfoBackground.setOutlineColor(sf::Color::Black);

	float shapeSize(WINDOWS_SIZE.y - mInfoBackground.getSize().y - OFFSET);
	mBattleField.setSize(sf::Vector2f(shapeSize, shapeSize));
	mBattleField.setPosition(
		(mInfoBackground.getSize().x - shapeSize) / 2,  mInfoBackground.getSize().y + OFFSET / 2);

	mGridLines[0].setSize(sf::Vector2f(shapeSize, 2.f));
	mGridLines[0].setOrigin(0, 1.f);
	mGridLines[0].setFillColor(sf::Color(20, 110, 90));
	mGridLines[1] = mGridLines[0];
	mGridLines[1].rotate(90);
	mGridLines[2].setSize(sf::Vector2f(shapeSize, 4.f));
	mGridLines[2].setOrigin(0, 2.f);
	mGridLines[2].setFillColor(sf::Color(10, 60, 50));
	mGridLines[3] = mGridLines[2];
	mGridLines[3].rotate(90);

	mSpaceBetweenThinLines = mBattleField.getSize().x / 9;
	mSpaceBetweenMainLines = mBattleField.getSize().x / 3;	

	mSelectedCellHightligh.setSize(
		sf::Vector2f(mSpaceBetweenMainLines, mSpaceBetweenMainLines));
	mSelectedCellHightligh.setFillColor(sf::Color(255, 255, 255, 160));

	mFont.loadFromFile("comicbd.ttf");
	mMarks[0].setString("X");
	mMarks[0].setFont(mFont);
	mMarks[0].setCharacterSize(FONT_SIZE);
	mMarks[0].setColor(sf::Color::Red);
	mMarks[1] = mMarks[0];
	mMarks[1].setString("O");
	mMarks[1].setColor(sf::Color::Blue);
	mMarks[2] = mMarks[0];
	mMarks[2].setCharacterSize(FONT_SIZE * 3);
	mMarks[2].setColor(sf::Color(190, 0, 0, 255));
	mMarks[3] = mMarks[1];
	mMarks[3].setCharacterSize(FONT_SIZE * 3);
	mMarks[3].setColor(sf::Color(0, 0, 190, 255));

	mChangePlayer.setString("Play with friend");
	mChangePlayer.setFont(mFont);
	mChangePlayer.setCharacterSize(FONT_SIZE / 2);
	mChangePlayer.setColor(sf::Color::Black);

	mStartNewGame.setString("Start new game");
	mStartNewGame.setFont(mFont);
	mStartNewGame.setCharacterSize(FONT_SIZE / 2);
	mStartNewGame.setColor(sf::Color::Black);
	mStartNewGame.setPosition(WINDOWS_SIZE.x - mStartNewGame.getGlobalBounds().width, 0);

	mGameOverMessage.setFont(mFont);

	mScoreText[0].setString("X: 0");
	mScoreText[0].setFont(mFont);
	mScoreText[0].setPosition(OFFSET / 2, OFFSET / 2);
	mScoreText[0].setColor(sf::Color::Red);
	mScoreText[1].setString("O: 0");
	mScoreText[1].setFont(mFont);
	mScoreText[1].setPosition(WINDOWS_SIZE.x - mScoreText[1].getGlobalBounds().width - 
		(OFFSET / 2), OFFSET / 2);
	mScoreText[1].setColor(sf::Color::Blue);

	mTurnText[0].setString("Turn X");
	mTurnText[0].setFont(mFont);
	mTurnText[0].setPosition((WINDOWS_SIZE.x - mTurnText[0].getGlobalBounds().width) / 2, 0);
	mTurnText[0].setColor(sf::Color::Red);
	mTurnText[1] = mTurnText[0];
	mTurnText[1].setString("Turn O");
	mTurnText[1].setColor(sf::Color::Blue);
}

void Game::run()
{
	while (mWindow.isOpen())
	{
		proccessInput();
		if (!mIsGameOver)
		{
			if (!mIsTurnX && mIsGameWithComputer)
				computerMove();
			else
				humanMove();
			update();
		}
		render();
	}
}

void Game::update()
{
	if (mIsPlayerReadyToMove)
	{
		EPlayer victory = checkVictory(mGameField[mSelectedCell.x]);
		if (victory != Empty)
			mWonCell[mSelectedCell.x] = victory;
		victory = checkVictory(mWonCell);
		if (victory != Empty)
		{
			mIsGameOver = true;

			switch (victory) {
			case Draw:
				mGameOverMessage.setString("Draw!");
				mGameOverMessage.setColor(sf::Color::Black);
				break;

			case PlayerX:
				mGameOverMessage.setString("X wins!");
				mGameOverMessage.setColor(sf::Color::Red);
				++mScore.x;
				mScoreText[0].setString("X: " + 
					std::to_string(static_cast<long long>(mScore.x)));	//
				mScoreText[0].setPosition(OFFSET / 2, OFFSET / 2);
				break;

			case PlayerO:
				mGameOverMessage.setString("O wins!");
				mGameOverMessage.setColor(sf::Color::Blue);
				++mScore.y;
				mScoreText[1].setString("O: " + 
					std::to_string(static_cast<long long>(mScore.y)));	//
				mScoreText[1].setPosition(WINDOWS_SIZE.x - mScoreText[1].getGlobalBounds().width - 
					(OFFSET / 2), OFFSET / 2);
				break;
			}

			mGameOverMessage.setPosition((WINDOWS_SIZE.x - 
				mGameOverMessage.getGlobalBounds().width) / 2, 0);
		}

		if (Empty == mWonCell[mSelectedCell.y])
			mSelectedCell.x = mSelectedCell.y;
		else
			mIsCellSelected = false;

		mSelectedCellHightligh.setPosition(
			mBattleField.getPosition().x + mSpaceBetweenMainLines * (mSelectedCell.x / 3),
			mBattleField.getPosition().y + mSpaceBetweenMainLines * (mSelectedCell.x % 3));
		mIsPlayerReadyToMove = false;

	}
}

void Game::proccessInput()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			mWindow.close();
		}

		if (isClickInZone(event, mChangePlayer.getGlobalBounds()))
		{
			mIsGameWithComputer = !mIsGameWithComputer;
			if (!mIsGameWithComputer)
				mChangePlayer.setString("Play with computer");
			else
				mChangePlayer.setString("Play with friend");
		}

		if (isClickInZone(event, mStartNewGame.getGlobalBounds()))
		{
			for (int i = 0; i < 9; ++i)
			{
				for (int j = 0; j < 9; ++j)
					mGameField[i][j] = Empty;
				mWonCell[i] = Empty;
			}

			mIsGameOver = false;
			mIsCellSelected = false;
			mIsTurnX = true;
			mIsPlayerReadyToMove = false;
		}

		if (!mIsGameOver &&
			isClickInZone(event, mBattleField.getGlobalBounds()))
		{
			int indexOfCell = findCellByClick(mSpaceBetweenMainLines, 0,
				sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

			if (!mIsCellSelected)
				mSelectedCell.x = indexOfCell;
			else if (indexOfCell != mSelectedCell.x || mWonCell[indexOfCell] != Empty)
				continue;

			mSelectedCell.y = findCellByClick(mSpaceBetweenThinLines, mSelectedCell.x, 
				sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
			
			mIsCellSelected = true;
			mIsPlayerReadyToMove = true;
		}
	}
}

void Game::render()
{
	mWindow.clear(sf::Color(40, 220, 180));
	mWindow.draw(mInfoBackground);
	mWindow.draw(mChangePlayer);
	mWindow.draw(mStartNewGame);
	mWindow.draw(mScoreText[0]);
	mWindow.draw(mScoreText[1]);

	if (mIsGameOver)
		mWindow.draw(mGameOverMessage);
	else if (mIsTurnX)
		mWindow.draw(mTurnText[0]);
	else
		mWindow.draw(mTurnText[1]);

	if (mIsCellSelected)
		mWindow.draw(mSelectedCellHightligh);

	drawLines(0, 10, mSpaceBetweenThinLines, false);
	drawLines(1, 10, mSpaceBetweenThinLines, true);
	drawLines(2, 4,  mSpaceBetweenMainLines, false);
	drawLines(3, 4,  mSpaceBetweenMainLines, true);

	sf::Text *mark(nullptr);
	for (int i = 0; i < 9; ++i)
	{
		for (int j = 0; j < 9; ++j)
		{
			if		(mGameField[i][j] == PlayerX)
				mark = &mMarks[0];
			else if (mGameField[i][j] == PlayerO)
				mark = &mMarks[1];
			else 
				continue;

			mark->setPosition(mBattleField.getPosition().x + (((i / 3) * 3 + (j / 3)) * 
							mSpaceBetweenThinLines) + mSpaceBetweenThinLines / 6,
							  mBattleField.getPosition().y + (((i % 3) * 3 + (j % 3)) * 
							mSpaceBetweenThinLines) - mSpaceBetweenThinLines / 12);
			mWindow.draw(*mark);
		}

		if		(mWonCell[i] == PlayerX)
			mark = &mMarks[2];
		else if (mWonCell[i] == PlayerO)
			mark = &mMarks[3];
		else 
			continue;

		mark->setPosition(mBattleField.getPosition().x + ((i / 3) * 
						mSpaceBetweenMainLines) + mSpaceBetweenThinLines / 2,
						  mBattleField.getPosition().y + ((i % 3) * 
						mSpaceBetweenMainLines) - mSpaceBetweenMainLines / 10);
		mWindow.draw(*mark);
	}

	mWindow.display();
}

void Game::drawLines(std::size_t lineIndex, int count, float space, bool isVertical)
{
	sf::Vector2f startPos(mBattleField.getPosition());
	for (int i = 0; i < count; ++i)
	{
		if (isVertical)
			mGridLines[lineIndex].setPosition(startPos.x + i * space, startPos.y);
		else
			mGridLines[lineIndex].setPosition(startPos.x, startPos.y + i * space);
		mWindow.draw(mGridLines[lineIndex]);	
	}
}

int Game::findCellByClick(float cellSize, int offset, sf::Vector2i mouseButtonPosition)
{
	return ((mouseButtonPosition.x - 
			static_cast<int>(mBattleField.getPosition().x)) / 
			static_cast<int>(cellSize) - (offset / 3) * 3) * 3  +

			((mouseButtonPosition.y - 
			static_cast<int>(mBattleField.getPosition().y)) / 
			static_cast<int>(cellSize) - (offset % 3) * 3);
}

Game::EPlayer Game::checkVictory(const std::array<EPlayer, 9> &field)
{
	for (int i = 0; i < 8; ++i)
	{
		if (field[winCombinationList[i][0]] != Empty &&
		    field[winCombinationList[i][0]] == field[winCombinationList[i][1]] && 
		    field[winCombinationList[i][1]] == field[winCombinationList[i][2]])

			return field[winCombinationList[i][0]];
	}
 
	int countOfFullCell(0);
	for (int i = 0; i < 9; ++i)
	{
		if (field[i] != Empty)
		++countOfFullCell;
	}
	if (countOfFullCell == 9) 
		return Draw;
 
	return Empty;
}

bool Game::isClickInZone(const sf::Event &event, const sf::FloatRect &zone)
{
	return (event.type == sf::Event::MouseButtonPressed &&
			zone.contains(static_cast<float>(event.mouseButton.x), 
						  static_cast<float>(event.mouseButton.y)));
}

void Game::humanMove()
{
	if (mIsPlayerReadyToMove)
	{
		if (!mIsTurnX)
			mGameField[mSelectedCell.x][mSelectedCell.y] = PlayerO;
		else
			mGameField[mSelectedCell.x][mSelectedCell.y] = PlayerX;
		mIsTurnX = !mIsTurnX;
	}
}

void Game::computerMove()
{
	if (!mIsCellSelected)
	{
		do
			mSelectedCell.x = rand() % 9;
		while(mWonCell[mSelectedCell.x] != Empty);
		mIsCellSelected = true;
	}

	do
		mSelectedCell.y = rand() % 9;
	while(mGameField[mSelectedCell.x][mSelectedCell.y] != Empty);

	mGameField[mSelectedCell.x][mSelectedCell.y] = PlayerO;
	mIsTurnX = !mIsTurnX;
	mIsPlayerReadyToMove = true;
}

int main()
{
	srand(static_cast<unsigned int>(time(0)));
	Game game;
	game.run();
	return 0;
}

/*
 
int main()
{
	EPlayer gameField[9][9]	= {Empty};
	EPlayer wonCell[9]		= {Empty};

	srand(static_cast<unsigned int>(time(0)));
	sf::RenderWindow window(sf::VideoMode(640, 480), "Tic Tac Toe ^2");
	window.setFramerateLimit(60);

	//info background (score, turn)
	RectangleShape infoBackground(Vector2f(
								  static_cast<float>(window.getSize().x), 
								  70));
	infoBackground.setFillColor(Color(240, 240, 240));
	infoBackground.setOutlineThickness(1.f);
	infoBackground.setOutlineColor(Color::Black);

	//game field zone
	float shapeSize(window.getSize().y - infoBackground.getSize().y - IDENT);
	RectangleShape shape(Vector2f(shapeSize, shapeSize));
	shape.setPosition((infoBackground.getSize().x - shape.getSize().x) / 2, 
					   infoBackground.getSize().y + IDENT / 2);

	//lines for game grid
	RectangleShape smalHorizontalLine(Vector2f(shapeSize, 2.f));
	smalHorizontalLine.setOrigin(0, smalHorizontalLine.getSize().y / 2);
	smalHorizontalLine.setFillColor(Color(20, 110, 90));
	RectangleShape smalVerticalLine(smalHorizontalLine);
	smalVerticalLine.rotate(90);
	RectangleShape bigHorizontalLine(Vector2f(shapeSize, 4.f));
	bigHorizontalLine.setOrigin(0, bigHorizontalLine.getSize().y / 2);
	bigHorizontalLine.setFillColor(Color(10, 60, 50));
	RectangleShape bigVerticalLine(bigHorizontalLine);
	bigVerticalLine.rotate(90);

	//space between game grid (need for calculation click position)
	float spaceBetweenThinLines(shape.getSize().x / 9);
	float spaceBetweenMainLines(shape.getSize().x / 3);						
	//Vector2f startPos(shape.getPosition().x,								
					 // shape.getPosition().y);

	//selected cell
	bool isCellSelected(false);
	int mainCell, subCell;
	RectangleShape cellSelected(Vector2f(spaceBetweenMainLines, 
										 spaceBetweenMainLines));
	cellSelected.setFillColor(Color(255, 255, 255, 160));

	//turn info
	bool isTurnX(true);
	Font font;
	font.loadFromFile("comicbd.ttf");
	Text turnX("Turn X", font), turnO("Turn O", font);
	turnX.setColor(Color::Red);
	turnO.setColor(Color::Blue);
	turnX.setPosition((window.getSize().x - turnX.getGlobalBounds().width) / 2, 0);
	turnO.setPosition((window.getSize().x - turnO.getGlobalBounds().width) / 2, 0);

	//victory info
	EPlayer victory;
	bool isGameOver(false);
	Text endWinX("X wins!", font), endWinO("O wins!", font), 
		 endDraw("Draw!", font), *gameOverMessage;
	endWinX.setColor(Color::Red);
	endWinO.setColor(Color::Blue);
	endDraw.setColor(Color::Black);
	endWinX.setPosition((window.getSize().x - endWinX.getGlobalBounds().width) / 2, 0);
	endWinO.setPosition((window.getSize().x - endWinO.getGlobalBounds().width) / 2, 0);
	endDraw.setPosition((window.getSize().x - endDraw.getGlobalBounds().width) / 2, 0);

	//option
	bool gameWithComputer(true);
	Text startNewGame("Start new game", font, 20u), playWithComputer("Play with computer", font, 20u);
	startNewGame.setColor(Color::Black);
	startNewGame.setPosition(window.getSize().x - 
							 startNewGame.getGlobalBounds().width, 0);
	playWithComputer.setColor(Color::Black);
	Text playWithFriend(playWithComputer);
	playWithFriend.setString("Play with friend");

	//score info
	std::size_t varScoreX(0), varScoreO(0);
	Text scoreX("X: 0", font), scoreO("O: 0", font);
	scoreX.setColor(Color::Red);
	scoreO.setColor(Color::Blue);
	scoreX.setPosition(IDENT / 2, 
					   IDENT / 2);
	scoreO.setPosition(window.getSize().x - scoreO.getGlobalBounds().width - (IDENT / 2), 
					   IDENT / 2);

	//mark on grid
	Text markX("X", font, FONT_SIZE), markO("O", font, FONT_SIZE), *mark;
	markX.setColor(Color::Red);
	markO.setColor(Color::Blue);
	markX.setOrigin(markX.getGlobalBounds().width / 2, markX.getGlobalBounds().height / 2);
	markO.setOrigin(markO.getGlobalBounds().width / 2, markO.getGlobalBounds().height / 2);

	Text bigMarkX(markX), bigMarkO(markO);
	bigMarkX.setCharacterSize(markX.getCharacterSize() * 3);
	bigMarkO.setCharacterSize(markO.getCharacterSize() * 3);
	bigMarkX.setColor(Color(190, 0, 0, 255));
	bigMarkO.setColor(Color(0, 0, 190, 255));
 
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

			if (event.type == Event::MouseButtonPressed)
			{
				if (startNewGame.getGlobalBounds().contains(
							static_cast<float>(event.mouseButton.x), 
							static_cast<float>(event.mouseButton.y)))
				{
					clearField(gameField, wonCell);
					isTurnX = true;
					isCellSelected = false;
					isGameOver = false;
				}

				if (((gameWithComputer)? 
					  playWithComputer : playWithFriend).getGlobalBounds().contains(
							static_cast<float>(event.mouseButton.x), 
							static_cast<float>(event.mouseButton.y)))
				{
					gameWithComputer = !gameWithComputer;
				}
			}

			if (event.type == Event::MouseButtonPressed &&
				event.mouseButton.button == Mouse::Left &&
				shape.getGlobalBounds().contains(
							static_cast<float>(event.mouseButton.x), 
							static_cast<float>(event.mouseButton.y)))
			{
				if (isGameOver)
					break;

				if (!isCellSelected)
				{
					mainCell = findCellByClick(shape.getPosition(), 
							   Vector2i(event.mouseButton.x, event.mouseButton.y), 
							   spaceBetweenMainLines);

					if (wonCell[mainCell] == Empty)
						isCellSelected = true;
					else
						break;
				}
				else if (cellSelected.getGlobalBounds().contains(
							static_cast<float>(event.mouseButton.x), 
							static_cast<float>(event.mouseButton.y)))
				{
					subCell = findCellByClick(cellSelected.getPosition(), 
								  Vector2i(event.mouseButton.x, event.mouseButton.y), 
								  spaceBetweenThinLines);

					if (gameField[mainCell][subCell])
						break;
					
					do
					{
						if (!isTurnX && gameWithComputer)
						{
							if (!isCellSelected)
								do
									mainCell = rand() % 9;
								while(wonCell[mainCell] != Empty);

							do
								subCell = rand() % 9;
							while(gameField[mainCell][subCell] != Empty);
						}

						if (isTurnX)
							gameField[mainCell][subCell] = PlayerX;
						else
							gameField[mainCell][subCell] = PlayerO;
						isTurnX = !isTurnX;

						victory = checkVictory(gameField[mainCell]);
						switch (victory) {
						case PlayerX:
							wonCell[mainCell] = PlayerX;
							break;
						case PlayerO:
							wonCell[mainCell] = PlayerO;
							break;
						case Draw:
							wonCell[mainCell] = Draw;
							break;
						}

						victory = checkVictory(wonCell);
						if (victory)
						{
							isGameOver = true;
							ostringstream score;
						
							switch (victory) {
							case PlayerX:
								gameOverMessage = &endWinX;
								score << ++varScoreX;
								scoreX.setString("X: " + score.str());
								break;
							case PlayerO:
								gameOverMessage = &endWinO;
								score << ++varScoreO;
								scoreO.setString("O: " + score.str());
								scoreO.setPosition(window.getSize().x - 
									scoreO.getGlobalBounds().width - (IDENT / 2), 
												   IDENT / 2);
								break;
							case Draw:
								gameOverMessage = &endDraw;
								break;
							}
						}

						if (wonCell[subCell] == Empty)
							mainCell = subCell;
						else
							isCellSelected = false;

					} while (!isTurnX && gameWithComputer);
				} 
				cellSelected.setPosition(shape.getPosition().x + 
										spaceBetweenMainLines * (mainCell / 3),
										 shape.getPosition().y + 
										spaceBetweenMainLines * (mainCell % 3));
			}
		}
		
		window.clear(Color(40, 220, 180));
		window.draw(infoBackground);
		if (!isGameOver)
			if (isTurnX)
				window.draw(turnX);
			else
				window.draw(turnO);
		else
			window.draw(*gameOverMessage);

		if (gameWithComputer)
			window.draw(playWithFriend);
		else
			window.draw(playWithComputer);

		window.draw(startNewGame);
		window.draw(scoreX);
		window.draw(scoreO);

		//window.draw(shape);
		if (isCellSelected)
			window.draw(cellSelected);
		drawLines(window, smalHorizontalLine, shape.getPosition(), 10, spaceBetweenThinLines, false);
		drawLines(window, smalVerticalLine,   shape.getPosition(), 10, spaceBetweenThinLines, true);
		drawLines(window, bigHorizontalLine,  shape.getPosition(), 4,  spaceBetweenMainLines, false);
		drawLines(window, bigVerticalLine,	  shape.getPosition(), 4,  spaceBetweenMainLines, true);

		for (int i = 0; i < 9; ++i)
		{
			for (int j = 0; j < 9; ++j)
			{
				if		(gameField[i][j] == PlayerX)
					mark = &markX;
				else if (gameField[i][j] == PlayerO)
					mark = &markO;
				else 
					continue;

				mark->setPosition(shape.getPosition().x + (((i / 3) * 3 + (j / 3)) * 
								spaceBetweenThinLines) + spaceBetweenThinLines / 2,
								  shape.getPosition().y + (((i % 3) * 3 + (j % 3)) * 
								spaceBetweenThinLines) + spaceBetweenThinLines / 4);
				window.draw(*mark);
			}

			if		(wonCell[i] == PlayerX)
				mark = &bigMarkX;
			else if (wonCell[i] == PlayerO)
				mark = &bigMarkO;
			else 
				continue;

			mark->setPosition(shape.getPosition().x + ((i / 3) * 
							spaceBetweenMainLines) + spaceBetweenMainLines / 4,
							  shape.getPosition().y + ((i % 3) * 
							spaceBetweenMainLines));
			window.draw(*mark);
		}
		window.display();
	}
	return 0;
}*/