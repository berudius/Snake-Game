#include <raylib.h>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>

using namespace std;
using namespace chrono;
using namespace this_thread;


enum Way
{
	Right = 1,
	Left = 2,
	Up = 3,
	Down = 4
};


const int windowWidth = 810;
const int windowHeight = 675;
const int cellWidth = 15;
const int cellHeight = 15;

const int quantityOfCells_X = windowWidth / cellWidth;
const int quantityOfCells_Y = windowHeight / cellHeight;

//head is righter
Vector2 in_center_tail = { ((quantityOfCells_X / 2 - 1) * cellWidth),  (quantityOfCells_Y / 2) * cellHeight };
Vector2 in_center_head = { ((quantityOfCells_X / 2 - 1) * cellWidth + cellWidth),  (quantityOfCells_Y / 2) * cellHeight };


int current_move = Right;//it should be in the snake class but I'm not sure about passing an instance of the class to a function that is running on another thread
int reverse_speed = 90;






template<typename T>
struct Node {
	T data;
	Node* next;

	Node(T val) : data(val), next(nullptr) {}
};

// Клас однонаправленого списку
template<typename T>
class SinglyLinkedList {
private:
	Node<T>* head;
	int size = 0;

public:
	SinglyLinkedList() : head(nullptr) {}

	// Додати елемент в кінець списку
	void append(T val) {
		Node<T>* newNode = new Node<T>(val);
		if (!head) {
			head = newNode;
		}
		else {
			Node<T>* current = head;
			while (current->next) {
				current = current->next;
			}
			current->next = newNode;
		}
		size++;
	}

	void re_write(int indx, T val) {
		Node<T>* current = head;
		int index = 0;
		while (current && index < indx) {
			current = current->next;
			index++;
		}
		if (current) {
			current->data = val;
		}
	}

	// Отримати елемент за індексом
	T getEl(int indx) {
		Node<T>* current = head;
		int index = 0;
		while (current && index < indx) {
			current = current->next;
			index++;
		}
		if (current) {
			return current->data;
		}
		else {
			// Повертаємо дефолтне значення у випадку, якщо елемент не знайдено
			return T();
		}
	}

	void clear() {
		Node<T>* current = head;
		while (current) {
			Node<T>* temp = current;
			current = current->next;
			delete temp;
		}
		head = nullptr;
		size = 0;
	}

	int getSize() {
		return size;
	}
};


class Snake
{
public:
	int Cell_resolution;
	Vector2 Header;
	SinglyLinkedList<Vector2> body = {};
	Vector2 food = {};
	int Score = 0;

	int step = 15;
	Sound eatSound;
	Sound deadSound;
	Sound pumpSound;

	Snake(int resolution, Vector2 head, Vector2 tail)
	{
		Cell_resolution = resolution;
		this->Header = head;
		body.append(tail);
		Vector2 vec = { tail.x - step,tail.y };
		Vector2 vec2 = { tail.x - 3,tail.y };
		body.append(vec);
		body.append(vec2);

		srand(time(NULL));
		food.x = rand() % 54 * step;
		food.y = rand() % 45 * step;

		InitAudioDevice();
		eatSound = LoadSound("Sounds/brue.mp3");
		deadSound = LoadSound("Sounds/winda.mp3");
		pumpSound = LoadSound("Sounds/pump.mp3");
	}


	void place_food()
	{
		srand(time(NULL));
		
		do 
		{
			food.x = rand() % 54 * step;
			food.y = rand() % 45 * step;
		} while (foodIsOnSnake());
	}

	bool food_IsEaten()
	{
		if (Header.x == food.x && Header.y == food.y)
		{
			PlaySound(eatSound);
			return true;
		}

		/*return false;*/
		return false;
	}

	void drawFood()
	{
		DrawRectangle(food.x, food.y, 15, 15, PINK);
	}
	

	void MoveRight()
	{
		MoveBody();
		
		Header.x += step;
		
		DrawRectangle((int)Header.x, (int)Header.y, Cell_resolution, Cell_resolution, YELLOW);
		
	}

	void MoveUp()
	{

		MoveBody();
		
		
		Header.y -= step;
		DrawRectangle((int)Header.x, (int)Header.y, Cell_resolution, Cell_resolution, YELLOW);
		
		
	}

	void MoveDown()
	{
		MoveBody();
		
		Header.y += step;
		DrawRectangle((int)Header.x, (int)Header.y, Cell_resolution, Cell_resolution, YELLOW);
	
	}

	void MoveLeft()
	{
		MoveBody();
		

		Header.x -= step;
		DrawRectangle((int)Header.x, (int)Header.y, Cell_resolution, Cell_resolution, YELLOW);

	}

	bool game_over()
	{
		for (int i = 0; i < body.getSize(); i++)
		{
			if (Header.x == body.getEl(i).x && Header.y == body.getEl(i).y)
			{
				return true;
			}
		}

		return false;
	}
	
	


private:

	bool foodIsOnSnake()
	{
		for (int i = 0; i < body.getSize(); i++)
		{
			if (food.x == body.getEl(i).x && food.y == body.getEl(i).y)
			{
				return true;
			}
		}

		return false;
	}

	void MoveBody()
	{
		Vector2 temp1 = body.getEl(0);
		Vector2 temp2 = {};
		body.re_write(0, Header);
		DrawRectangle(body.getEl(0).x, body.getEl(0).y, Cell_resolution, Cell_resolution, GREEN);

		for (int i = 1; i < body.getSize(); i++)
		{
			if (i % 2 != 0)
			{
				temp2 = body.getEl(i);
				body.re_write(i, temp1);
			}
			else
			{
				temp1 = body.getEl(i);
				body.re_write(i, temp2);
			}
			DrawRectangle(body.getEl(i).x, body.getEl(i).y, Cell_resolution, Cell_resolution, GREEN);
		}
	}
};





void CheckPressingButtons()
{
	while (true)
	{
		if (IsKeyDown(KEY_RIGHT) && current_move != Left)
		{
			//snake.MoveRight();
			current_move =  Right;
			while (IsKeyDown(KEY_RIGHT)) {}
		}

		if (IsKeyDown(KEY_LEFT) && current_move != Right)
		{
			//snake.MoveLeft();
			current_move = Left;
			while (IsKeyDown(KEY_LEFT)) {}
		}

		if (IsKeyDown(KEY_UP) && current_move != Down)
		{
			//snake.MoveUp();
			current_move = Up;
			while (IsKeyDown(KEY_UP)) {}
		}

		if (IsKeyDown(KEY_DOWN) && current_move != Up)
		{
			//snake.MoveDown();
			current_move = Down;
			while (IsKeyDown(KEY_DOWN)){}
		}

		if (IsKeyDown(KEY_LEFT_SHIFT))
		{
			reverse_speed = 130;
		}

		else 
		{
			reverse_speed = 90;
		}
		
	}
}

int main()
{
	Snake snake(cellWidth, in_center_head,in_center_tail);
	Color bgColor = { 61,121,134,0};

	InitWindow(windowWidth, windowHeight, "Snake");
	SetTargetFPS(60);


	
	thread th(CheckPressingButtons);
	snake.place_food();
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(bgColor);



		//string text = std::to_string(snake.Header.x) + "\n" + std::to_string(snake.Header.y);
	 //   int length = text.length() + 1;
		//char *arr = new char[length];
		//strcpy_s(arr, length, text.c_str());
		//DrawText(arr, 0, 0,15, BLACK);
		//delete[]arr;

		string text = to_string(snake.Score);
		int length = text.length() + 1;
		char *arr = new char[length];
		strcpy_s(arr, length, text.c_str());
		DrawText(arr, 10, 10,15, BLACK);
		delete[]arr;
 		
		if (snake.game_over())
		{
			snake.body.clear();
			snake.Header = in_center_head;
			snake.body.append(in_center_tail);
			snake.Score = 0;

			PlaySound(snake.pumpSound);
		}


		switch ((int)snake.Header.x)
		{
		case -15:
			snake.Header.x = windowWidth - 15;
			break;
		case windowWidth:
			snake.Header.x = 0;
			break;
		default:
			break;
		}

		switch ((int)snake.Header.y)
		{
		case -15:
			snake.Header.y = windowHeight - 15;
			break;
		case windowHeight:
			snake.Header.y = 0;
			break;
		default:
			break;
		}


		if (snake.food_IsEaten())
		{

			snake.Score += 7;
			snake.place_food();
			int size = snake.body.getSize();

			Vector2 tail = snake.body.getEl(size - 1);
			switch (current_move)
			{
				case Right:
					tail.x -= 15;
					snake.body.append(tail);
					break;
				case Left:
					tail.x += 15;
					snake.body.append(tail);
					break;
				case Up:
					tail.y += 15;
					snake.body.append(tail);
					break;
				case Down:
					tail.y -= 15;
					snake.body.append(tail);
				default:
					break;
			}

		}
		
		snake.drawFood();

		switch (current_move)
		{
		case Right:
			snake.MoveRight();
			break;
		case Left:
			snake.MoveLeft();
			break;
		case Up:
			snake.MoveUp();
			break;
		case Down:
			snake.MoveDown();
			break;
		default:
			break;
		}


		

		EndDrawing();
		this_thread::sleep_for(milliseconds(reverse_speed));
	}

	th.join();

	CloseWindow();

	return 0;
}
