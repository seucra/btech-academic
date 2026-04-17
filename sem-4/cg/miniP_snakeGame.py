import pygame
import random
import sys

# Constants mapped directly from your C code
SIZE = 16
FPS = 8
WIDTH = 480
HEIGHT = 480
CELL_SIZE = WIDTH // SIZE

# Colors mapped from glUniform3f calls
BG_COLOR = (51, 51, 51)       # 0.2, 0.2, 0.2
SNAKE_COLOR = (51, 255, 51)   # 0.2, 1.0, 0.2
APPLE_COLOR = (255, 51, 51)   # 1.0, 0.2, 0.2

def init_game():
    # Head at (SIZE-1)>>1, which is 7. Array stores (x, y) tuples.
    # Appending 3 body parts to the left as per the C loop.
    snake = [(7, 7), (6, 7), (5, 7), (4, 7)]
    dx, dy = 0, 0
    
    # Place initial apple randomly
    while True:
        apple = (random.randint(0, SIZE - 1), random.randint(0, SIZE - 1))
        if apple not in snake:
            break
            
    return snake, apple, dx, dy, False

def main():
    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("Zoro Snake")
    clock = pygame.time.Clock()

    snake, apple, dx, dy, gameover = init_game()

    while True:
        # Event Polling
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
                
        keys = pygame.key.get_pressed()
        
        if keys[pygame.K_ESCAPE] or keys[pygame.K_q]:
            pygame.quit()
            sys.exit()
            
        if gameover and keys[pygame.K_r]:
            snake, apple, dx, dy, gameover = init_game()

        # Input mapping (Exactly matching the C conditional quirk)
        if not gameover:
            if keys[pygame.K_d] or keys[pygame.K_RIGHT]:
                if dx == 0: dx, dy = 1, 0
            if keys[pygame.K_a] or keys[pygame.K_LEFT]:
                if dx == 0 and dy != 0: dx, dy = -1, 0
            if keys[pygame.K_w] or keys[pygame.K_UP]:
                if dy == 0: dx, dy = 0, -1
            if keys[pygame.K_s] or keys[pygame.K_DOWN]:
                if dy == 0: dx, dy = 0, 1

        # Game Logic Update
        if not gameover and (dx != 0 or dy != 0):
            head_x, head_y = snake[0]
            new_head = (head_x + dx, head_y + dy)

            # Check boundaries
            if new_head[0] < 0 or new_head[0] >= SIZE or new_head[1] < 0 or new_head[1] >= SIZE:
                gameover = True
            # Check self-collision (excluding the tail tip since it moves forward)
            elif new_head in snake[:-1]:
                gameover = True
            else:
                snake.insert(0, new_head) # Move head
                
                if new_head == apple:
                    # Apple eaten: don't remove tail, generate new apple
                    while True:
                        apple = (random.randint(0, SIZE - 1), random.randint(0, SIZE - 1))
                        if apple not in snake:
                            break
                else:
                    # Normal movement: remove tail
                    snake.pop()

        # Render
        screen.fill(BG_COLOR)
        
        # In the C code, s = 1.8 / SIZE. This leaves a 10% gap (5% padding per side)
        padding = CELL_SIZE * 0.05
        block_size = CELL_SIZE * 0.9

        # Draw Apple
        pygame.draw.rect(screen, APPLE_COLOR, 
                         (apple[0] * CELL_SIZE + padding, 
                          apple[1] * CELL_SIZE + padding, 
                          block_size, block_size))
        
        # Draw Snake
        for segment in snake:
            pygame.draw.rect(screen, SNAKE_COLOR, 
                             (segment[0] * CELL_SIZE + padding, 
                              segment[1] * CELL_SIZE + padding, 
                              block_size, block_size))

        pygame.display.flip()
        clock.tick(FPS)

if __name__ == "__main__":
    main()
