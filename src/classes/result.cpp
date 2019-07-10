class Result
{
    static Result &instance()
    {
        static Result result;
        return result;
    }
};