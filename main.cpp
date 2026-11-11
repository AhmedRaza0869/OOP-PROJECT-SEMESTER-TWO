#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

Color green={173, 204, 96, 255};
Color darkgreen={43, 51, 24, 255};
Color pink={252, 33, 132, 255};

int cellsize=30;
int cellcount=25;
int offset=75;

double lastupdatetime=0;

bool eventtriggered(double interval)
{
    double currenttime=GetTime();
    if(currenttime-lastupdatetime>=interval)
    {
        lastupdatetime=currenttime;
        return true;
    }
    return false;
}

bool elementindeque(Vector2 element, deque<Vector2> deque)
{
    for(unsigned int i=0;i<deque.size();i++)
    {
        if(Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}

class Food
{
    public: 
        Vector2 position;   
        Texture2D texture;

        Food(deque<Vector2> snakebody)
        {
            position=RandomFood(snakebody);
        }

        void draw()
        {
            DrawRectangle(offset+position.x*cellsize, offset+position.y*cellsize, cellsize, cellsize, darkgreen);
        }

        Vector2 RandomFood(deque<Vector2> snakebody)
        {
            float x=GetRandomValue(0, cellcount-1);
            float y=GetRandomValue(0, cellcount-1);
            
            Vector2 position={x,y};
            
            while(elementindeque(position, snakebody))
            {
                float x=GetRandomValue(0, cellcount-1);
                float y=GetRandomValue(0, cellcount-1);
                
                Vector2 position={x,y};
            }

            return position;
        }

        ~Food()
        {
            UnloadTexture(texture);
        }
};

class Snake
{
    public:
        deque<Vector2> body={Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
        Vector2 direction={1,0};
        bool addsegment=false;
        
        
        void draw()
        {
            for(unsigned i=0; i<body.size(); i++)
            {
                float x=body[i].x;
                float y=body[i].y;
                
                Rectangle segment=Rectangle{offset+x*cellsize, offset+y*cellsize, (float)cellsize, (float)cellsize};
                DrawRectangleRounded(segment, 0.5, 6, pink);
            }
        }
        
        void update()
        {
            body.push_front(Vector2Add(body[0],direction));
            if(addsegment==true)
            {
                addsegment=false;
            }
            else
            {
                body.pop_back();
            }
        }
            
        void reset()
        {
            if(eventtriggered(10))  // not working
            {
                DrawText("Game Over!", 375, 375, 20, darkgreen);
            }
            body={Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
            direction={1,0};
        }
};
    
class Game
{
    public:
        Snake snake=Snake();
        Food food=Food(snake.body);
        bool running=true;
        int score=0;
        Sound eat;
        Sound wall;

        Game()
        {
            InitAudioDevice();
            eat=LoadSound("sound/eat.wav");
            wall=LoadSound("sound/wall.mp3");
        }

        void draw()
        {
            food.draw();
            snake.draw();
        }
        
        void update()
        {
            if(running)
            {
                snake.update();
                CheckSnakeHasEatenFood();
                CheckCollisionWithBorder();
                CheckCollisionWithBody();
            }
        }
        
        void CheckSnakeHasEatenFood()
        {
            if(Vector2Equals(snake.body[0], food.position))
            {
                cout<<"Eating Food"<<endl;
                food.position=food.RandomFood(snake.body);
                snake.addsegment=true;
                score+=1;
                PlaySound(eat);
            }
        }
        
        void CheckCollisionWithBorder()
        {
            if(snake.body[0].x==cellcount || snake.body[0].x==-1)
            { 
                GameOver();
            }
            if(snake.body[0].y==cellcount || snake.body[0].y==-1)
            {
                GameOver();
            }
        }
        
        void CheckCollisionWithBody()
        {
            deque<Vector2> headlessBody=snake.body;
            headlessBody.pop_front();
            
            if(elementindeque(snake.body[0],headlessBody))
            {
                GameOver();
            }
        }
        
        void GameOver()
        {
            PlaySound(wall);
            snake.reset();
            food.position=food.RandomFood(snake.body);
            running=false;
            score=0;
        }

        ~Game()
        {
            UnloadSound(eat);
            UnloadSound(wall);
            CloseAudioDevice();
        }
};

int main()
{
    InitWindow(2*offset+750, 2*offset+750, "Snakester");  
    
    SetTargetFPS(60);
    
    Game game=Game();
    
    while(WindowShouldClose()==false)
    {
        BeginDrawing();
        
        if(eventtriggered(0.2))
        {
            game.update();
        }

        if( (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && game.snake.direction.y!=1)
        {
            game.snake.direction={0,-1};
            game.running=true;
        }

        if( (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && game.snake.direction.y!=-1)
        {
            game.snake.direction={0,1};
            game.running=true;
        }
        
        if( (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && game.snake.direction.x!=-1)
        {
            game.snake.direction={1,0};
            game.running=true;
        }
        
        if( (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && game.snake.direction.x!=1)
        {
            game.snake.direction={-1,0};
            game.running=true;
        }


        ClearBackground(green);

        DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)760, (float)760}, 5, darkgreen);

        DrawText("Snakester", 375, 10, 40, darkgreen);

        DrawText("Score: ", 630, 830, 40, darkgreen);
        DrawText(TextFormat("%i", game.score), 810, 830, 40, darkgreen);

        game.draw();

        EndDrawing();
    }

    CloseWindow();
}