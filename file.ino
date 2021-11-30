#include <FastLED.h>

struct Game
{
    static const uint8_t datapin = 7;
    static const uint8_t field_length = 144;
    static const uint32_t button_timeout = 500;
    CRGB leds[Game::field_length];
    uint8_t maxpoints = 3;
    float speed = 10;
    float pos = 0;
    float prevframe = 0;
    bool direction = true;
};

struct Player {
    uint8_t bpin;
    uint32_t time_stamp = 0;
    bool bpressed = false;
    uint8_t points = 0;
    Player(uint8_t pbpin){
        bpin = pbpin;
    }
};

Player player1(2);
Player player2(3);
Game game;

void draw(){
    uint8_t pos = (uint8_t) game.pos;
    if (game.pos == 0) game.leds[game.field_length - 1] = CRGB::Black;
    else if (game.pos == game.field_length - 1) game.leds[0] = CRGB::Black;
    if (game.speed > 0) game.leds[pos - 1] = CRGB::Black;
    else game.leds[pos + 1] = CRGB::Black;
    game.leds[pos] = CRGB::Amethyst;
    FastLED.show();
}

void readbuttons(Player &player){
    if (millis() - player.time_stamp > game.button_timeout){
        if (digitalRead(player.bpin) == HIGH) {
            player.bpressed = true;
            player.time_stamp = millis();
        }
        else player.bpressed = false;
    }
    else player.bpressed = false;
}

void readbuttons(){
    readbuttons(player1);
    readbuttons(player2);
}

void setup(){
    game = Game();
    pinMode(player1.bpin, INPUT);
    pinMode(player2.bpin, INPUT);
    FastLED.addLeds<NEOPIXEL, Game::datapin>(game.leds, game.field_length);
}

void loop(){
    readbuttons();
    if (player1.bpressed) game.speed = -game.speed;
    if (player2.bpressed) game.speed = -game.speed;
    uint32_t time_diff = millis() - game.prevframe;
    float dis_inc = game.speed / 1000 * time_diff;
    game.pos = game.pos + dis_inc;
    if (game.pos >= game.field_length) game.pos = 0;
    if (game.pos < 0) game.pos = game.field_length - 1;
    draw();
    game.prevframe = millis();
    delay(5);
}