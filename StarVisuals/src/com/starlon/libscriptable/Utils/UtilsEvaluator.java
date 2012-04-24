package com.starlon.libscriptable;

public class UtilsEvaluator {
    private CPtr Evaluator;

    public static native CPtr newEvaluator();
    public static native void delEvaluator(CPtr eval);
    public static native String evaluate(CPtr eval, String str);

    public void UtilsEvaluator() {
        Evaluator = newEvaluator();
    }

    public String eval(String str)
    {
        return evaluate(Evaluator, str);
    }

    @Override
    public void finalize()
    {
        delEvaluator(Evaluator);
    }
}
