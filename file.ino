#include <FastLED.h>

struct Game
{
    static const uint8_t datapin = 7;
    static const uint8_t field_length = 144;
    static const uint32_t button_timeout = 2000;
    static const uint8_t border = 8;
    CRGB field_color = CRGB{16,0,0};
    CRGB point_color = CRGB{32, 32, 32};
    CRGB leds[Game::field_length];
    uint8_t maxpoints = 3;
    float speed = 15;
    float pos = 0;
    float prevframe = 0;
    bool direction = true;
};

struct Player {
    uint8_t bpin;
    uint32_t time_stamp = 0;
    bool inrange = false;
    bool bpressed = false;
    uint8_t points = 0;
    Player(uint8_t pbpin){
        bpin = pbpin;
    }
};

Player player1(2);
Player player2(3);
Game game;

void drawPoint(){
    // TODO: Letzten Pixel nicht immer Schwarz sodern letzter Zustand (==> Rand)
    uint8_t pos = (uint8_t) game.pos;
    // Wenn 1. LED dann letzte LED auf Schwarz
    if (game.pos == 0) game.leds[game.field_length - 1] = CRGB::Black;
    // Wenn letzte LED dann 1. LED auf Schwarz
    else if (game.pos == game.field_length - 1) game.leds[0] = CRGB::Black;
    // Wenn wir nach oben laufen setze vorherige LED auf Schwarz
    if (game.speed > 0 && game.pos > 1) game.leds[pos - 1] = CRGB::Black;
    else if (game.pos < game.field_length - 1) game.leds[pos + 1] = CRGB::Black;
    game.leds[pos] = game.point_color;
}

void drawField(){
    for (int i = 0; i < game.border; i++) game.leds[i] = game.field_color;
    for (int i = game.field_length - 1; i >= game.field_length - game.border; i--) game.leds[i] = game.field_color;
}

void draw(){
    drawField();
    drawPoint();
    FastLED.show();
    // Serial.println(game.foo);
}

void playerinput(){
    player1.inrange = (game.pos < game.border);
    player2.inrange = (game.field_length - game.pos <= game.border);
}

void readbuttons(Player &player){
    if (millis() - player.time_stamp > game.button_timeout){
        if (digitalRead(player.bpin) == HIGH) {
            playerinput();
            if (player.inrange) player.bpressed = true;
            else player.bpressed = false;
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
    Serial.begin(9600);
    game = Game();
    game.field_color = game.field_color.lerp8(CRGB::Black, 64);
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