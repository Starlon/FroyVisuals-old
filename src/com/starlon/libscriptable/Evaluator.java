package com.starlon.libscriptable;

import org.mozilla.javascript.*;

public class Evaluator {
    Context mContext;
    Scriptable mScope;
    public Evaluator()
    {
        mContext = Context.enter();

        // Initialize the standard objects (Object, Function, etc.)
        // This must be done before scripts can be executed. Returns
        // a scope object that we use in later calls.
        mScope = mContext.initStandardObjects();
    }
    public void exit()
    {
        Context.exit();
    }
    public String Evaluate(String script)
    {
        String res;
        try {

            // Now evaluate the string we've colected.
            Object result = mContext.evaluateString(mScope, script, "<cmd>", 1, null);

            // Convert the result to a string and print it.
            res = Context.toString(result);

        } finally {
        }
        return res;
    }
}

