void setup(){
    Serial1.begin(9600);
}

enum States { Idle, Seek, Approach, Grip, Lift, MoveToDrop, Release } state;

void Tick(){
    switch(state){  //state transitions
        case Idle:
            break;
        case Seek:
            break;
        case Approach:
            break;
        case Grip:
            break;
        case Lift:
            break;
        case MoveToDrop:
            break;
        case Release:
            break;
        default:
            break;
    }
    switch(state){  //state actions
        case Idle:
            break;
        case Seek:
            break;
        case Approach:
            break;
        case Grip:
            break;
        case Lift:
            break;
        case MoveToDrop:
            break;
        case Release:
            break;
        default:
            break;
    }
  }

void loop() {
    state = Idle;
    Tick();
}
