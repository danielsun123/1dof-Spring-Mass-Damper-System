#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include <SDL_ttf.h>
#include <string.h>
#include <pthread.h>

#include <sys/time.h>
#include <Python.h>

//define macros
#define SCREEN_WIDTH 640.0
#define SCREEN_HEIGHT 540.0

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

// we are assuming the x-axis to be positive going vertically downwards

// Define parameters and initial conditions
double m = 1.0;            // Mass (kg)
double k = 15.0;           // Spring constant (N/m)
double c = 0.65;           // Damping coefficient

double x0 = 0.5;           // Initial displacement from equilibrium (m)  for eductional purposes, we assume x0 <------ 0 = scenario where the string is unstretched and let go,  x plotted on graphs will be distance from unstretched position
double v0 = 0.0;           // Initial velocity (m/s)

double g = 9.81;     //gravitational acceleration (m/s^2)
double dt = 0.01;          // Time step

double t, x, v, a;   // global var

double time_limit = 5.0;   // run program until 5 seconds reached 

SDL_Event event; 
int exit_flag = 0;   


bool create_window_and_renderer() {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    else { 
        window = SDL_CreateWindow("Spring mass damping system", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            return 1;
        }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == NULL) {
            printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
            return 1;
        }
    }
}

void *simulate_spring(void *arg) {
 
    (void)arg;
    
    double xi;  

    double x_eq = (m * g) / k;   // stretched amount (x_eq) of spring when in equilibrium

     if (x0 > 0 && x0 < x_eq) {        // assuming x_m must be less than x_eq
        xi = x_eq + x0;
    }
    else if (x0 < 0 && x0 > -1 * x_eq) {
        xi = x_eq + x0;
    }
    else if (x0 == 0){ 
        xi = x0;
    } else {
        printf("not a valid x0");
        pthread_exit(NULL);;
    }
    
    double x_temp = xi;  // temporary storage 
    
    x = x_temp;   // Current position
    v = v0;   // Current velocity
    t = 0;

    TTF_Font* font = NULL;         
    SDL_Color textColor = {255, 255, 255}; // White color

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        printf("TTF_Init failed: %s\n", TTF_GetError());
        // Handle initialization error if needed
    }

    // Load a font (replace "path_to_your_font.ttf" with the actual path)
    font = TTF_OpenFont("C:/Users/danie/Desktop/Spring_Mass_Damping_System/Spring_Mass_Damping_System_Final/MwQ5bhbm2POE6VhLPJp6qGI.ttf", 35); // 35 is the font size

    remove("data.txt");
    FILE *file = fopen("data.txt", "w");
      if (file == NULL) {
        fprintf(stderr, "Error opening file for writing\n");
        pthread_exit(NULL);
    }

     while (!exit_flag) { 
    
        fprintf(file,"%f %f %f ", t, x, v);
        fflush(file);   

        // --------------------drawing objects--------------------

         // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        // indicator lines
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);    // Set the line color to white
        int y1 = floor(225 + 65.4);
        int y2 = floor(225 + 65.4);
        SDL_RenderDrawLine(renderer, 0, y1, 350, y2);      // white: equilibrium line, kx = mg (x = 0.645)
        
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderDrawLine(renderer, 0, 225, 350, 225);       // green: unstretched spring position (x = 0)

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderDrawLine(renderer, 0, 225, 350, 225);       // blue (top): min compressed spring position (-x_m: -0.654 max initial displacement from equilibrium)
        SDL_RenderDrawLine(renderer, 0, 356, 350, 356);     // blue (bottom): max stretched spring position (+x_m: +0.654 max initial displacement from equilibrium)
         
        // Draw the spring
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);   

        double freq = 0.25;    // designated frequency 
        double amplitude = 10;  // width of spring
        int spring_original_length = 200;

        int spring_length = spring_original_length + (x * 100);
        double new_freq = (spring_original_length * freq) / (spring_original_length + (x * 100));

        for (int y = 0; y < spring_length; y++) {
            double x_spring = 150 + amplitude * sin(new_freq * y);
            SDL_RenderDrawPoint(renderer, (int)x_spring, (int)y);
        }

        // Draw the block (mass)
        SDL_Rect rect = {125, 225 + (int)(x * 100) - 25, 50, 50}; // x, y, width, height                 
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red color
        SDL_RenderFillRect(renderer, &rect);

         // --------------------drawing objects--------------------

        char timeString[100], positionString[100], velocityString[100], accelerationString[100];    

        sprintf(timeString, "Time(s): %.2f", t); 
        sprintf(positionString, "Pos(m): %.2f", x); 
        sprintf(velocityString, "Vel(m/s): %.2f", v); 

        // Forces acting on mass
        double F_spring = -k * x;         //spring force
        double F_gravity = m * g;         //gravitational force
        double F_damping = -c * v;        //damping force

        // Calculate the total force
        double F_total = F_spring + F_gravity + F_damping;
        //double F_total = F_spring + F_gravity;

        // Calculate the acceleration
        double a = F_total / m;

        // Update velocity and position using Euler's method
        v += a * dt;
        x += v * dt;

        // Update time
        t += dt;

        fprintf(file,"%f\n", a);
        fflush(file);   

        sprintf(accelerationString, "Acc(m/s^2): %.2f", a); 
    
        // Render text surfaces
        SDL_Surface* timeSurface = TTF_RenderText_Solid(font, timeString, textColor);
        SDL_Surface* positionSurface = TTF_RenderText_Solid(font, positionString, textColor);
        SDL_Surface* velocitySurface = TTF_RenderText_Solid(font, velocityString, textColor);
        SDL_Surface* accelerationSurface = TTF_RenderText_Solid(font, accelerationString, textColor);

        // Create texture from surfaces
        SDL_Texture* timeTexture = SDL_CreateTextureFromSurface(renderer, timeSurface);
        SDL_Texture* positionTexture = SDL_CreateTextureFromSurface(renderer, positionSurface);
        SDL_Texture* velocityTexture = SDL_CreateTextureFromSurface(renderer, velocitySurface);
        SDL_Texture* accelerationTexture = SDL_CreateTextureFromSurface(renderer, accelerationSurface);

         // Get the width of your window or renderer
        int textureWidth = timeSurface->w; // Assuming timeSurface holds the texture's width

        // Calculate the x coordinate for the top-right corner
        int xCoordinate = SCREEN_WIDTH - textureWidth;

        // Render textures to the screen
        SDL_Rect dstRect = {xCoordinate, 0, textureWidth, timeSurface->h}; // Adjust position as needed
        SDL_RenderCopy(renderer, timeTexture, NULL, &dstRect);
        dstRect.y += timeSurface->h + 5; // Adjust vertical spacing

        SDL_RenderCopy(renderer, positionTexture, NULL, &dstRect);
        dstRect.y += positionSurface->h + 5;

        SDL_RenderCopy(renderer, velocityTexture, NULL, &dstRect);
        dstRect.y += velocitySurface->h + 5;

        SDL_RenderCopy(renderer, accelerationTexture, NULL, &dstRect);
        
        SDL_RenderPresent(renderer);  

         // Free surfaces and textures
        SDL_FreeSurface(timeSurface);
        SDL_FreeSurface(positionSurface);
        SDL_FreeSurface(velocitySurface);
        SDL_FreeSurface(accelerationSurface);
        
        SDL_DestroyTexture(timeTexture);
        SDL_DestroyTexture(positionTexture);
        SDL_DestroyTexture(velocityTexture);
        SDL_DestroyTexture(accelerationTexture);

        SDL_Delay(10);

    }

    pthread_exit(NULL);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    TTF_CloseFont(font);
    TTF_Quit(); 

}


void *plot_data(void *arg) {
    
    (void)arg; // Unused argument

    FILE *gnuplot_pipe = popen("gnuplot -persistent", "w");
    if (gnuplot_pipe == NULL) {
        fprintf(stderr, "Could not open pipe to Gnuplot\n");
        pthread_exit(NULL); ;
    }

    fprintf(gnuplot_pipe, "set xlabel \"time (s)\"\n");
    fprintf(gnuplot_pipe, "plot 'data.txt' u 1:2 w lines title 'position (m)', '' u 1:3 w lines title 'velocity (m/s)', '' u 1:4 w lines title 'acceleration (m/s^2)'\n");  

    while (!exit_flag) {
        fprintf(gnuplot_pipe, "replot\n");
        fflush(gnuplot_pipe);
    }
    
    pclose(gnuplot_pipe);
    system("taskkill /F /IM gnuplot_qt.exe");
    pthread_exit(NULL);

}



// ----------------------------main code to execute------------------------------

int main(int argc, char *argv[]) {

    create_window_and_renderer();  // create window and renderer
    
    pthread_t calcThread, plotThread; // separate threads 

    pthread_create(&calcThread, NULL, simulate_spring, NULL);
    pthread_create(&plotThread, NULL, plot_data, NULL);

    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
                exit_flag = 1;  // Make sure to set the exit flag in the main thread as well
            }
        }
    }

    // Wait for threads to finish (need to implement a termination condition)
    pthread_join(calcThread, NULL);
    pthread_join(plotThread, NULL);
   
    return 0;
}

















