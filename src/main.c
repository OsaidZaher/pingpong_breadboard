#include <stm32f031x6.h>
#include "display.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#define WINNING_SCORE 7 

#define FRAME_INTERVAL 16  // ~60fps
volatile uint32_t lastDrawTime = 0;

void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);

volatile uint32_t milliseconds;
volatile uint8_t gameMode = 0;

// Sprite declarations
const  uint16_t bat1[] = {0,0,0,0,0,8,88,144,144,112,24,0,0,0,0,0,0,0,0,0,32,184,200,200,200,200,200,72,0,0,0,0,0,0,0,24,184,200,200,200,200,200,200,200,56,0,0,0,0,0,0,120,200,200,200,200,200,200,200,200,168,0,0,0,0,0,0,184,200,200,200,200,200,200,200,200,200,32,0,0,0,0,0,200,200,200,200,200,200,200,200,200,200,48,0,0,0,0,0,168,200,200,200,200,200,200,200,200,200,24,0,0,0,0,0,96,200,200,200,200,200,200,200,200,144,0,0,0,0,0,0,8,160,200,200,200,200,200,200,184,32,0,0,0,0,0,0,0,8,144,200,200,200,200,168,32,0,0,0,0,0,0,0,0,0,0,32,51505,51505,8752,0,0,0,0,0,0,0,0,0,0,0,0,0,19457,19457,41728,0,0,0,0,0,0,0,0,0,0,0,0,0,19457,19457,41728,0,0,0,0,0,0,0,0,0,0,0,0,0,19457,19457,41728,0,0,0,0,0,0,0,0,0,0,0,0,0,19457,19457,41728,0,0,0,0,0,0,0,0,0,0,0,0,0,9728,9728,49408,0,0,0,0,0,0,};
const  uint16_t bat2[] = {0,0,0,0,0,256,2816,4608,4608,3584,768,0,0,0,0,0,0,0,0,0,1024,5888,6400,6400,6400,6400,6400,2304,0,0,0,0,0,0,0,768,5888,6400,6400,6400,6400,6400,6400,6400,1792,0,0,0,0,0,0,3840,6400,6400,6400,6400,6400,6400,6400,6400,5376,0,0,0,0,0,0,5888,6400,6400,6400,6400,6400,6400,6400,6400,6400,1024,0,0,0,0,0,6400,6400,6400,6400,6400,6400,6400,6400,6400,6400,1536,0,0,0,0,0,5376,6400,6400,6400,6400,6400,6400,6400,6400,6400,768,0,0,0,0,0,3072,6400,6400,6400,6400,6400,6400,6400,6400,4608,0,0,0,0,0,0,256,5120,6400,6400,6400,6400,6400,6400,5888,1024,0,0,0,0,0,0,0,256,4608,6400,6400,6400,6400,5376,1024,0,0,0,0,0,0,0,0,0,0,1024,52993,52993,10240,0,0,0,0,0,0,0,0,0,0,0,0,0,19457,19457,41728,0,0,0,0,0,0,0,0,0,0,0,0,0,19457,19457,41728,0,0,0,0,0,0,0,0,0,0,0,0,0,19457,19457,41728,0,0,0,0,0,0,0,0,0,0,0,0,0,19457,19457,41728,0,0,0,0,0,0,0,0,0,0,0,0,0,9728,9728,49408,0,0,0,0,0,0,};
const  uint16_t pingpong_ball[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,528,3699,4228,1057,0,0,0,0,0,0,0,0,0,0,0,528,63950,65535,65535,65007,1585,0,0,0,0,0,0,0,0,0,0,3699,65535,65535,65535,65535,62893,0,0,0,0,0,0,0,0,0,0,4228,65535,65535,65535,65535,63421,0,0,0,0,0,0,0,0,0,0,1057,65007,65535,65535,65535,2642,0,0,0,0,0,0,0,0,0,0,0,1585,62893,63421,2642,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
#define BALL_SIZE 16        // Ball sprite is 16x16
#define PADDLE_WIDTH 16     // Paddle width based on sprite
#define PADDLE_HEIGHT 16    // Paddle height based on sprite
#define SCREEN_WIDTH 128    // Display width
#define SCREEN_HEIGHT 160   // Display height
#define PADDLE_SPEED 3      // Pixels per movement
#define BALL_SPEED_X 1     // Horizontal ball speed
#define BALL_SPEED_Y 1      // Vertical ball speed
#define AI_DIFFICULTY_EASY 1
#define AI_DIFFICULTY_MEDIUM 2
#define AI_DIFFICULTY_HARD 3
#define AI_BASE_SPEED 2


volatile uint8_t aiDifficulty = AI_DIFFICULTY_MEDIUM;  // Default to medium

// Game objects
typedef struct {
    int x;
    int y;
    int dx;
    int dy;
    int active;  // Flag for ball movement
} Ball;

typedef struct {
    int x;
    int y;
    int score;
} Paddle;

Ball ball;
Paddle leftPaddle;
Paddle rightPaddle;

// Initialize game state
void initGame() {
    ball.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
    ball.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
    ball.dx = BALL_SPEED_X;
    ball.dy = BALL_SPEED_Y;
    ball.active = 1;

    leftPaddle.x = 10;
    leftPaddle.y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    leftPaddle.score = 0;

    rightPaddle.x = SCREEN_WIDTH - 10 - PADDLE_WIDTH;
    rightPaddle.y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    rightPaddle.score = 0;
}

void showStartupScreen() {
    // Menu state variables
    int currentSelection = 0;  // 0: 2P, 1: 1P Easy, 2: 1P Medium, 3: 1P Hard
    int lastButtonState = 1;   // For button debouncing
    const char* options[] = {"2P", "1P-E", "1P-M", "1P-H"};  // Shortened text
    const int numOptions = 4;
    bool needsRedraw = true;   // Only redraw when necessary
    
    // Initial screen draw
    fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    
    while (1) {
        if (needsRedraw) {
            // Draw static elements
            printText("Mode", SCREEN_WIDTH/2 - 15, 10, RGBToWord(255, 255, 255), RGBToWord(0, 0, 0));
            printText("^:chg >:sel", SCREEN_WIDTH/2 - 25, 20, RGBToWord(200, 200, 200), RGBToWord(0, 0, 0));

            // Display all options in a compact layout
            for (int i = 0; i < numOptions; i++) {
                uint16_t color = (i == currentSelection) ? RGBToWord(0, 255, 0) : RGBToWord(255, 255, 255);
                printText(options[i], SCREEN_WIDTH/2 - 15, 35 + (i * 10), color, RGBToWord(0, 0, 0));
            }
            
            needsRedraw = false;
        }

        // Check for button press (PA8) with debouncing
        int currentButtonState = (GPIOA->IDR & (1 << 8)) == 0;
        if (currentButtonState && !lastButtonState) {
            currentSelection = (currentSelection + 1) % numOptions;
            needsRedraw = true;
            delay(150);  // Reduced debounce delay
        }
        lastButtonState = currentButtonState;

        // Check for confirmation (PB4)
        if ((GPIOB->IDR & (1 << 4)) == 0) {
            // Set game mode and AI difficulty based on selection
            if (currentSelection == 0) {
                gameMode = 0;  // 2P mode
                fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
                printText("2P", SCREEN_WIDTH/2 - 10, SCREEN_HEIGHT/2, RGBToWord(0, 255, 0), RGBToWord(0, 0, 0));
            } else {
                gameMode = 1;  // 1P mode
                fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
                switch (currentSelection) {
                    case 1:
                        aiDifficulty = AI_DIFFICULTY_EASY;
                        printText("1P-Easy", SCREEN_WIDTH/2 - 20, SCREEN_HEIGHT/2, RGBToWord(0, 255, 0), RGBToWord(0, 0, 0));
                        break;
                    case 2:
                        aiDifficulty = AI_DIFFICULTY_MEDIUM;
                        printText("1P-Med", SCREEN_WIDTH/2 - 20, SCREEN_HEIGHT/2, RGBToWord(0, 255, 0), RGBToWord(0, 0, 0));
                        break;
                    case 3:
                        aiDifficulty = AI_DIFFICULTY_HARD;
                        printText("1P-Hard", SCREEN_WIDTH/2 - 20, SCREEN_HEIGHT/2, RGBToWord(0, 255, 0), RGBToWord(0, 0, 0));
                        break;
                }
            }
            delay(800);
            break;
        }
    }
    
    fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
}

void updateAI() {
    float aiSpeed;
    int predictionError;
    
    // Set AI characteristics based on difficulty
    switch(aiDifficulty) {
        case AI_DIFFICULTY_EASY:
            aiSpeed = AI_BASE_SPEED * 0.7f;
            predictionError = 20;  // Larger error margin
            break;
            
        case AI_DIFFICULTY_MEDIUM:
            aiSpeed = AI_BASE_SPEED;
            predictionError = 10;  // Medium error margin
            break;
            
        case AI_DIFFICULTY_HARD:
            aiSpeed = AI_BASE_SPEED * 1.3f;
            predictionError = 0;   // No error
            break;
            
        default:
            aiSpeed = AI_BASE_SPEED;
            predictionError = 10;
    }

    // Calculate where the ball will intersect with the paddle's x position
    int predictedY = ball.y + (ball.dy * (rightPaddle.x - ball.x) / ball.dx);
    
    // Add random error based on difficulty
    if (predictionError > 0) {
        predictedY += (rand() % (predictionError * 2)) - predictionError;
    }
    
    // Move paddle towards predicted position
    int targetY = predictedY - (PADDLE_HEIGHT / 2);
    
    // Add delay for easier difficulties by only moving sometimes
    if (aiDifficulty == AI_DIFFICULTY_EASY && (rand() % 3 != 0)) {
        return;  // Skip movement sometimes for easy mode
    }
    
    if (rightPaddle.y < targetY) {
        rightPaddle.y += aiSpeed;
    } else if (rightPaddle.y > targetY) {
        rightPaddle.y -= aiSpeed;
    }
    
    // Ensure paddle stays within screen bounds
    if (rightPaddle.y < 0) rightPaddle.y = 0;
    if (rightPaddle.y > SCREEN_HEIGHT - PADDLE_HEIGHT) {
        rightPaddle.y = SCREEN_HEIGHT - PADDLE_HEIGHT;
    }
}

// Update ball position and handle collisions
void updateBall() {
    if (!ball.active) return;

    ball.x += ball.dx;
    ball.y += ball.dy;

    // Bounce off top and bottom walls
    if (ball.y <= 0 || ball.y >= SCREEN_HEIGHT - BALL_SIZE) {
        ball.dy = -ball.dy;
        ball.y = (ball.y <= 0) ? 0 : SCREEN_HEIGHT - BALL_SIZE;
    }

    // Paddle collision check
    if (isInside(leftPaddle.x, leftPaddle.y, PADDLE_WIDTH, PADDLE_HEIGHT, ball.x, ball.y)) {
        ball.dx = BALL_SPEED_X;
        ball.dy = ((ball.y + BALL_SIZE/2) - (leftPaddle.y + PADDLE_HEIGHT/2)) / 4;
    }
    if (isInside(rightPaddle.x, rightPaddle.y, PADDLE_WIDTH, PADDLE_HEIGHT, ball.x + BALL_SIZE, ball.y)) {
        ball.dx = -BALL_SPEED_X;
        ball.dy = ((ball.y + BALL_SIZE/2) - (rightPaddle.y + PADDLE_HEIGHT/2)) / 4;
    }

    // Scoring and resetting the ball if it goes out of bounds
    if (ball.x <= 0 || ball.x >= SCREEN_WIDTH - BALL_SIZE) {
        if (ball.x <= 0) rightPaddle.score++;
        else leftPaddle.score++;
        
        ball.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
        ball.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
        ball.dx = (ball.x <= 0) ? BALL_SPEED_X : -BALL_SPEED_X;
        ball.active = 0;
    }

    // Check for a winner
    if (leftPaddle.score >= WINNING_SCORE || rightPaddle.score >= WINNING_SCORE) {
        ball.active = 0;
    }
}

// Draw game state



void selectGameMode() {
    // Simple game mode selection logic
    // Set gameMode to 1 for single-player, 0 for two-player
    if ((GPIOA->IDR & (1 << 11)) == 0) {  // Assume button at PA11
        gameMode = 1;  // Single-player (AI)
    } else {
        gameMode = 0;  // Two-player
    }
}
void drawGame() {
    static int lastBallX = -1;
    static int lastBallY = -1;
    static int lastLeftPaddleY = -1;
    static int lastRightPaddleY = -1;
    
    // Only update score area every few frames to reduce flickering
    static uint32_t lastScoreUpdate = 0;
    if (milliseconds - lastScoreUpdate > 100) {  // Update scores every 100ms
        // Clear and draw score area
        fillRectangle(0, 0, SCREEN_WIDTH, 20, 0);
        
        char score_str[5];
        sprintf(score_str, "%d", leftPaddle.score);
        printText(score_str, SCREEN_WIDTH/2 - 20, 5, RGBToWord(255, 255, 255), RGBToWord(0, 0, 0));
        sprintf(score_str, "%d", rightPaddle.score);
        printText(score_str, SCREEN_WIDTH/2 + 15, 5, RGBToWord(255, 255, 255), RGBToWord(0, 0, 0));
        
        lastScoreUpdate = milliseconds;
    }

    if (leftPaddle.score < WINNING_SCORE && rightPaddle.score < WINNING_SCORE) {
        // Draw center line (only for game area, below score area)
        for (int y = 20; y < SCREEN_HEIGHT; y += 8) {  // Start from y=20 to skip score area
            fillRectangle(SCREEN_WIDTH/2 - 1, y, 2, 4, RGBToWord(128, 128, 128));
        }

        // Clear previous ball position if it has moved
        if (lastBallX != -1 && (lastBallX != ball.x || lastBallY != ball.y)) {
            fillRectangle(lastBallX, lastBallY, BALL_SIZE, BALL_SIZE, 0);
        }
        
        // Clear previous paddle positions if they have moved
        if (lastLeftPaddleY != -1 && lastLeftPaddleY != leftPaddle.y) {
            fillRectangle(leftPaddle.x, lastLeftPaddleY, PADDLE_WIDTH, PADDLE_HEIGHT, 0);
        }
        if (lastRightPaddleY != -1 && lastRightPaddleY != rightPaddle.y) {
            fillRectangle(rightPaddle.x, lastRightPaddleY, PADDLE_WIDTH, PADDLE_HEIGHT, 0);
        }

        // Draw new positions
        putImage(leftPaddle.x, leftPaddle.y, PADDLE_WIDTH, PADDLE_HEIGHT, bat1, 0, 0);
        putImage(rightPaddle.x, rightPaddle.y, PADDLE_WIDTH, PADDLE_HEIGHT, bat2, 0, 0);
        putImage(ball.x, ball.y, BALL_SIZE, BALL_SIZE, pingpong_ball, 0, 0);
        
        // Store current positions
        lastBallX = ball.x;
        lastBallY = ball.y;
        lastLeftPaddleY = leftPaddle.y;
        lastRightPaddleY = rightPaddle.y;
    } else {
        // Game over screen
        fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        char* winner_text = (leftPaddle.score >= WINNING_SCORE) ? "Blue Wins!" : "Red Wins!";
        printText(winner_text, SCREEN_WIDTH/2 - 30, SCREEN_HEIGHT/2, RGBToWord(255, 255, 255), RGBToWord(0, 0, 0));
         printText("Press button to restart", SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT/2 + 10, RGBToWord(200, 200, 200), RGBToWord(0, 0, 0));
        
    }
}
void restartGame() {
    // Reset game state
    initGame();

    // Clear the screen and show the startup menu
    fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    showStartupScreen();

    // Reset last draw time to ensure proper game loop timing
    lastDrawTime = milliseconds;
}

int main() {
    initClock();
    initSysTick();
    setupIO();
    
    // Clear screen once at startup
    fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    showStartupScreen();
    initGame();

    uint32_t last_ball_reset = 0;
    
    while (1) {
    uint32_t currentTime = milliseconds;
    
    // Update game state at fixed intervals
    if (currentTime - lastDrawTime >= FRAME_INTERVAL) {
        // Game logic
        if (gameMode == 0) {
            if ((GPIOB->IDR & (1 << 5)) == 0 && rightPaddle.y < SCREEN_HEIGHT - PADDLE_HEIGHT) 
                rightPaddle.y += PADDLE_SPEED;
            if ((GPIOB->IDR & (1 << 4)) == 0 && rightPaddle.y > 0) 
                rightPaddle.y -= PADDLE_SPEED;
        } else {
            updateAI();
        }

        if ((GPIOA->IDR & (1 << 11)) == 0 && leftPaddle.y < SCREEN_HEIGHT - PADDLE_HEIGHT) 
            leftPaddle.y += PADDLE_SPEED;
        if ((GPIOA->IDR & (1 << 8)) == 0 && leftPaddle.y > 0) 
            leftPaddle.y -= PADDLE_SPEED;

        if (!ball.active && (currentTime - last_ball_reset > 1000)) {
            ball.active = 1;
            last_ball_reset = currentTime;
        }
        
        updateBall();
        drawGame();
        
        lastDrawTime = currentTime;
    } else {
        // Sleep while waiting for next frame
        __asm(" wfi ");
    }

    // Check for game over and restart
    if (leftPaddle.score >= WINNING_SCORE || rightPaddle.score >= WINNING_SCORE) {
        if ((GPIOB->IDR & (1 << 4)) == 0) {
            // Restart button pressed
            restartGame();
            showStartupScreen();
            break;
        }
    }
}
    return 0;
}
void initSysTick(void)
{
	SysTick->LOAD = 48000;
	SysTick->CTRL = 7;
	SysTick->VAL = 10;
	__asm(" cpsie i "); // enable interrupts
}
void SysTick_Handler(void)
{
	milliseconds++;
}
void initClock(void)
{
// This is potentially a dangerous function as it could
// result in a system with an invalid clock signal - result: a stuck system
        // Set the PLL up
        // First ensure PLL is disabled
        RCC->CR &= ~(1u<<24);
        while( (RCC->CR & (1 <<25))); // wait for PLL ready to be cleared
        
// Warning here: if system clock is greater than 24MHz then wait-state(s) need to be
// inserted into Flash memory interface
				
        FLASH->ACR |= (1 << 0);
        FLASH->ACR &=~((1u << 2) | (1u<<1));
        // Turn on FLASH prefetch buffer
        FLASH->ACR |= (1 << 4);
        // set PLL multiplier to 12 (yielding 48MHz)
        RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
        RCC->CFGR |= ((1<<21) | (1<<19) ); 

        // Need to limit ADC clock to below 14MHz so will change ADC prescaler to 4
        RCC->CFGR |= (1<<14);

        // and turn the PLL back on again
        RCC->CR |= (1<<24);        
        // set PLL as system clock source 
        RCC->CFGR |= (1<<1);
}
void delay(volatile uint32_t dly)
{
	uint32_t end_time = dly + milliseconds;
	while(milliseconds != end_time)
		__asm(" wfi "); // sleep
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	/*
	*/
	uint32_t mode_value = Port->MODER;
	Mode = Mode << (2 * BitNumber);
	mode_value = mode_value & ~(3u << (BitNumber * 2));
	mode_value = mode_value | Mode;
	Port->MODER = mode_value;
}
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py)
{
	// checks to see if point px,py is within the rectange defined by x,y,w,h
	uint16_t x2,y2;
	x2 = x1+w;
	y2 = y1+h;
	int rvalue = 0;
	if ( (px >= x1) && (px <= x2))
	{
		// ok, x constraint met
		if ( (py >= y1) && (py <= y2))
			rvalue = 1;
	}
	return rvalue;
}

void setupIO()
{
	RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
	display_begin();
	pinMode(GPIOB,4,0);
	pinMode(GPIOB,5,0);
	pinMode(GPIOA,8,0);
	pinMode(GPIOA,11,0);
	enablePullUp(GPIOB,4);
	enablePullUp(GPIOB,5);
	enablePullUp(GPIOA,11);
	enablePullUp(GPIOA,8);
}