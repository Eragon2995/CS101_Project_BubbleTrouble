#include <simplecpp>

/* Simulation Vars */
const double STEP_TIME = 0.02;
const double TOTAL_TIME = 30.00;
const int INITIAL_HEALTH = 3;

/* Game Vars */
const int PLAY_Y_HEIGHT = 450;
const int LEFT_MARGIN = 70;
const int TOP_MARGIN = 20;
const int BOTTOM_MARGIN = (PLAY_Y_HEIGHT+TOP_MARGIN);

bool is_shooter_being_hit = false;
double SCORE = 0;

#include "shooter.h"
#include "bubble.h"

void bullet_hit(vector<Bubble> &bubbles, vector<Bullet> &bullets)
{   // Check for bubbles hitting a bullet
    for (unsigned int i=0; i < bubbles.size(); i++){

        for(unsigned int j=0; j<bullets.size(); j++){

            double distance = bullets[j].get_width()/2 + bubbles[i].get_radius();
            double center_distance = sqrt(pow(bullets[j].get_center_x() - bubbles[i].get_center_x(), 2) + pow(bullets[j].get_center_y() - bubbles[i].get_center_y(), 2));
            // Condition for hitting bubble
            if(center_distance < distance)
            {
                // Erase bullet after hitting bubble
                bullets.erase(bullets.begin()+j);
                SCORE += 10;

                if(abs(bubbles[i].get_radius() - BUBBLE_DEFAULT_RADIUS) > 0.1)
                {
                    // Create new bubbles after original is hit
                    bubbles.push_back(Bubble(bubbles[i].get_center_x(), bubbles[i].get_center_y(), bubbles[i].get_radius()/2., -bubbles[i].get_vx()*2., -BUBBLE_DEFAULT_VY, bubbles[i].get_color()));
                    bubbles.push_back(Bubble(bubbles[i].get_center_x(), bubbles[i].get_center_y(), bubbles[i].get_radius()/2., bubbles[i].get_vx()*2., -BUBBLE_DEFAULT_VY, bubbles[i].get_color()));
                }

                // Erase original bubble
                bubbles.erase(bubbles.begin()+i);

            }
        }
    }
}

bool level_won(vector<Bubble> &bubbles)
{   // Check if all bubbles in level are hit
    if(bubbles.size() == 0)
        return true;
    return false;
}

bool shooter_hit(Shooter &shooter, vector<Bubble> &bubbles)
{   // Check for bubbles hitting shooter

    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        double distance_body = shooter.get_body_width()/2 + bubbles[i].get_radius();
        double center_distance_body = sqrt(pow(shooter.get_body_center_x() - bubbles[i].get_center_x(), 2) + pow(shooter.get_body_center_y() - bubbles[i].get_center_y(), 2));

        double distance_head = shooter.get_head_radius() + bubbles[i].get_radius();
        double center_distance_head = sqrt(pow(shooter.get_head_center_x() - bubbles[i].get_center_x(), 2) + pow(shooter.get_head_center_y() - bubbles[i].get_center_y(), 2));

        // Condition for hitting a bubble
        if(center_distance_head < distance_head || center_distance_body < distance_body)
        {
            // Shooter hit by bubble
            if(!is_shooter_being_hit)
            {
                is_shooter_being_hit = true;
                shooter.change_color(COLOR(255, 0, 0));
                SCORE -= 10;
                return true;
            }
            return false;

        }
    }
    is_shooter_being_hit = false;
    shooter.change_color(COLOR(0, 255, 0));
    return false;

}

void move_bullets(vector<Bullet> &bullets){
    // move all bullets
    for(unsigned int i=0; i<bullets.size(); i++){
        if(!bullets[i].nextStep(STEP_TIME)){
            bullets.erase(bullets.begin()+i);
        }
    }
}

void move_bubbles(vector<Bubble> &bubbles){
    // move all bubbles
    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        bubbles[i].nextStep(STEP_TIME);
    }
}

vector<Bubble> create_bubbles(int n, Color color)
{
    // create initial bubbles in nth level
    vector<Bubble> bubbles;
    int sign = -1;
    for(int i = 1; i <= n+1; i++)
    {
        sign *= -1;
        bubbles.push_back(Bubble(i*WINDOW_X/(n+2), BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS*pow(2, n-1), sign*BUBBLE_DEFAULT_VX*i/pow(2., n-1)/n+1, BUBBLE_DEFAULT_VY, color));
    }
    return bubbles;
}


int main()
{
    initCanvas("Bubble Trouble", WINDOW_X, WINDOW_Y);

    Line b1(0, PLAY_Y_HEIGHT, WINDOW_X, PLAY_Y_HEIGHT);
    b1.setColor(COLOR(0, 0, 255));

    string msg_cmd("Cmd: _");
    Text charPressed(LEFT_MARGIN, BOTTOM_MARGIN, msg_cmd);

    // Intialize the shooter
    Shooter shooter(SHOOTER_START_X, SHOOTER_START_Y, SHOOTER_VX);

    // Simulate levels
    for(int level = 1; level <= 3; level++)
    {
        string level_name;
        //Color of the bubbles
        Color color;
        if(level == 1)
        {
            level_name = "Level 1";
            color = COLOR(255,0, 255);
        }

        if(level == 2)
        {
            level_name = "Level 2";
            color = COLOR(0, 255, 255);
        }

        if(level == 3)
        {
            level_name = "Level 3";
            color = COLOR(255, 140, 0);
        }

        {
            Text level(WINDOW_X/2, PLAY_Y_HEIGHT/2, level_name);
            wait(2);
        }

        // Initialize the bubbles
        vector<Bubble> bubbles = create_bubbles(level, color);

        // Initialize the bullets (empty)
        vector<Bullet> bullets;

        XEvent event;

        double TIME_LEFT = TOTAL_TIME;
        int HEALTH = INITIAL_HEALTH;

        Text time_indicator(LEFT_MARGIN - 25, TOP_MARGIN, "Time left: ");
        Text time_left(LEFT_MARGIN + 25, TOP_MARGIN, TOTAL_TIME);

        Text health_indicator(WINDOW_X/2 - 18, BOTTOM_MARGIN, "Lives: ");
        Text health(WINDOW_X/2 + 8, BOTTOM_MARGIN, INITIAL_HEALTH);

        Text score_indicator(WINDOW_X - LEFT_MARGIN - 18, BOTTOM_MARGIN, "Score: ");
        Text score(WINDOW_X - LEFT_MARGIN + 16, BOTTOM_MARGIN, SCORE);

        // Main game loop
        while (true)
        {
            bool pendingEvent = checkEvent(event);
            if (pendingEvent)
            {
                // Get the key pressed
                char c = charFromEvent(event);
                msg_cmd[msg_cmd.length() - 1] = c;
                charPressed.setMessage(msg_cmd);

                // Update the shooter
                if(c == 'a')
                    shooter.move(STEP_TIME, true);
                else if(c == 'd')
                    shooter.move(STEP_TIME, false);
                else if(c == 'w')
                    bullets.push_back(shooter.shoot());
                else if(c == 'q')
                    return 0;

            }

            // Check for bullet hit
            bullet_hit(bubbles, bullets);

            // Check for shooter hit
            if(shooter_hit(shooter, bubbles))
            {
                HEALTH--;
            }

            // Check for level won
            if(level_won(bubbles))
            {
                Text level_won(WINDOW_X/2, PLAY_Y_HEIGHT/2, "LEVEL WON!");

                while(TIME_LEFT != 0)
                {
                    // Convert time left to score
                    TIME_LEFT -= STEP_TIME;
                    if(TIME_LEFT <= 2)
                        TIME_LEFT = 0;
                    SCORE += STEP_TIME*5;
                    time_left.reset(LEFT_MARGIN + 25, TOP_MARGIN, TIME_LEFT);
                    score.reset(WINDOW_X - LEFT_MARGIN + 16, BOTTOM_MARGIN, SCORE);

                }
                SCORE = (int)SCORE;
                wait(1);
                break;
            }

            //Timer
            TIME_LEFT -= STEP_TIME;
            if(TIME_LEFT <= 0.02)
                TIME_LEFT = 0;

            time_left.reset(LEFT_MARGIN + 25, TOP_MARGIN, TIME_LEFT);

            //Health counter
            health.reset(WINDOW_X/2 + 8, BOTTOM_MARGIN, HEALTH);

            //Score counter
            score.reset(WINDOW_X - LEFT_MARGIN + 16, BOTTOM_MARGIN, SCORE);


            //Check if game is lost
            if(HEALTH == 0 || TIME_LEFT  == 0)
            {
                Text game_over(WINDOW_X/2, PLAY_Y_HEIGHT/2 - 15, "GAME OVER!");
                Text your_score(WINDOW_X/2 - 6, PLAY_Y_HEIGHT/2 + 15, "Your score: ");
                Text final_score(WINDOW_X/2 + 46, PLAY_Y_HEIGHT/2 + 15, SCORE);
                wait(4);
                return 0;
            }


             // Update the bubbles
            move_bubbles(bubbles);

            // Update the bullets
            move_bullets(bullets);

            wait(STEP_TIME);

        }
    }

    Text game_won(WINDOW_X/2, PLAY_Y_HEIGHT/2 - 15, "YOU WON THE GAME!");
    Text your_score(WINDOW_X/2 - 6, PLAY_Y_HEIGHT/2 + 15, "Your score: ");
    Text final_score(WINDOW_X/2 + 46, PLAY_Y_HEIGHT/2 + 15, SCORE);
    wait(3);

}
