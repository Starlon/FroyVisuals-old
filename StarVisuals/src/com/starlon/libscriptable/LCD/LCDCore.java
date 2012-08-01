package com.starlon.libscriptable;

public class LCDCore {
    public static final int TRANSITION_LEFT = 0;
    public static final int TRANSITION_RIGHT = 1;
    public static final int TRANSITION_HBOTH = 2;
    public static final int TRANSITION_UP = 3;
    public static final int TRANSITION_DOWN = 4;
    public static final int TRANSITION_VBOTH = 5;
    public static final int TRANSITION_TENTACLE = 6;
    public static final int TRANSITION_CHECKERS = 7;

    class defaults {
        public static final int rows = 4;
        public static final int cols = 20;
        public static final int transition_timeout = 0;
        public static final int transition_speed = 100;
        public static final int timeout = 100;
    }

    public LCDCore(Object environment, String name, Object config, int typeOf, int errorLevel)
    {

    }

    
}
