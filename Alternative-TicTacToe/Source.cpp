#include <array>
#include <SFML/Graphics.hpp>

const int OFFSET(40);
const int FONT_SIZE(40);
const sf::Vector2u WINDOWS_SIZE(640, 480);

const int winCombinationList[8][3] = {
	{ 0, 1, 2 },{ 3, 4, 5 },{ 6, 7, 8 },
	{ 0, 3, 6 },{ 1, 4, 7 },{ 2, 5, 8 },
	{ 0, 4, 8 },{ 2, 4, 6 }
};

class Game
{
public:
	Game();
	void run();

private:
	enum EPlayer 
	{
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
	EPlayer checkVictory(const std::array<EPlayer, 9>& field);
	bool isClickInZone(const sf::Event& event, const sf::FloatRect& zone);

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
	mWindow(sf::VideoMode(WINDOWS_SIZE.x, WINDOWS_SIZE.y), "Alternative-TicTacToe",
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
		(mInfoBackground.getSize().x - shapeSize) / 2, mInfoBackground.getSize().y + OFFSET / 2);

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
	mMarks[0].setFillColor(sf::Color::Red);
	mMarks[1] = mMarks[0];
	mMarks[1].setString("O");
	mMarks[1].setFillColor(sf::Color::Blue);
	mMarks[2] = mMarks[0];
	mMarks[2].setCharacterSize(FONT_SIZE * 3);
	mMarks[2].setFillColor(sf::Color(190, 0, 0, 255));
	mMarks[3] = mMarks[1];
	mMarks[3].setCharacterSize(FONT_SIZE * 3);
	mMarks[3].setFillColor(sf::Color(0, 0, 190, 255));

	mChangePlayer.setString("Play with friend");
	mChangePlayer.setFont(mFont);
	mChangePlayer.setCharacterSize(FONT_SIZE / 2);
	mChangePlayer.setFillColor(sf::Color::Black);

	mStartNewGame.setString("Start new game");
	mStartNewGame.setFont(mFont);
	mStartNewGame.setCharacterSize(FONT_SIZE / 2);
	mStartNewGame.setFillColor(sf::Color::Black);
	mStartNewGame.setPosition(WINDOWS_SIZE.x - mStartNewGame.getGlobalBounds().width, 0);

	mGameOverMessage.setFont(mFont);

	mScoreText[0].setString("X: 0");
	mScoreText[0].setFont(mFont);
	mScoreText[0].setPosition(OFFSET / 2, OFFSET / 2);
	mScoreText[0].setFillColor(sf::Color::Red);
	mScoreText[1].setString("O: 0");
	mScoreText[1].setFont(mFont);
	mScoreText[1].setPosition(WINDOWS_SIZE.x - mScoreText[1].getGlobalBounds().width -
		(OFFSET / 2), OFFSET / 2);
	mScoreText[1].setFillColor(sf::Color::Blue);

	mTurnText[0].setString("Turn X");
	mTurnText[0].setFont(mFont);
	mTurnText[0].setPosition((WINDOWS_SIZE.x - mTurnText[0].getGlobalBounds().width) / 2, 0);
	mTurnText[0].setFillColor(sf::Color::Red);
	mTurnText[1] = mTurnText[0];
	mTurnText[1].setString("Turn O");
	mTurnText[1].setFillColor(sf::Color::Blue);
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
				mGameOverMessage.setFillColor(sf::Color::Black);
				break;

			case PlayerX:
				mGameOverMessage.setString("X wins!");
				mGameOverMessage.setFillColor(sf::Color::Red);
				++mScore.x;
				mScoreText[0].setString("X: " +
					std::to_string(static_cast<long long>(mScore.x)));	//
				mScoreText[0].setPosition(OFFSET / 2, OFFSET / 2);
				break;

			case PlayerO:
				mGameOverMessage.setString("O wins!");
				mGameOverMessage.setFillColor(sf::Color::Blue);
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
	drawLines(2, 4, mSpaceBetweenMainLines, false);
	drawLines(3, 4, mSpaceBetweenMainLines, true);

	sf::Text *mark(nullptr);
	for (int i = 0; i < 9; ++i)
	{
		for (int j = 0; j < 9; ++j)
		{
			if (mGameField[i][j] == PlayerX)
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

		if (mWonCell[i] == PlayerX)
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
		static_cast<int>(cellSize) - (offset / 3) * 3) * 3 +

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
		while (mWonCell[mSelectedCell.x] != Empty);
		mIsCellSelected = true;
	}

	do
		mSelectedCell.y = rand() % 9;
	while (mGameField[mSelectedCell.x][mSelectedCell.y] != Empty);

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