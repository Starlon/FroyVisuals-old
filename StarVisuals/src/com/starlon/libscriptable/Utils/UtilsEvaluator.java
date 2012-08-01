package com.starlon.libscriptable;

public class UtilsEvaluator {
    private CPtr Evaluator;

    public static native CPtr evaluatorNew();
    public static native void evaluatorDel(CPtr eval);
    public static native String evaluate(CPtr eval, String str);

    public UtilsEvaluator() {
        Evaluator = evaluatorNew();
    }

    public String eval(String str)
    {
        return evaluate(Evaluator, str);
    }

    @Override
    public void finalize()
    {
        evaluatorDel(Evaluator);
    }
}
