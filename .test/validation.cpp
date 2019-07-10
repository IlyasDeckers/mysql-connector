static void validateRow(std::auto_ptr< sql::ResultSet > & res, struct _test_data *exp)
{
    stringstream msg;

    cout << "#\t\t Fetching the first row, id = " << res->getInt("id");
    cout << ", label = '" << res->getString("label") << "'" << endl;

    if ((res->getInt("id") != exp->id) || (res->getString("label") != exp->label)) {
        msg.str("Wrong results");
        msg << "Expected (" << exp->id << "," << exp->label << ")";
        msg << " got (" << res->getInt("id") <<", " << res->getString("label") << ")";
        throw runtime_error(msg.str());
    }
}

static void validateResultSet(std::auto_ptr< sql::ResultSet > & res, struct _test_data *min, struct _test_data *max) {
    size_t row;

    cout << "#\t Selecting in ascending order but fetching in descending (reverse) order" << endl;
    /* Move the cursor after the last row - n + 1 */
    res->afterLast();
    if (true != res->isAfterLast())
        throw runtime_error("Position should be after last row (1)");

    row = res->rowsCount() - 1;
    /* Move the cursor backwards to: n, n - 1, ... 1, 0. Return true if rows are available. */
    while (res->previous()) {
        cout << "#\t\t Row " << row << " id = " << res->getInt("id");
        cout << ", label = '" << res->getString("label") << "'" << endl;
        row--;
    }
    /*
    The last call to res->previous() has moved the cursor before the first row
    Cursor position is 0, recall: rows are from 1 ... n
    */
    cout << "#\t\t isBeforeFirst() = " << res->isBeforeFirst() << endl;
    cout << "#\t\t isFirst() = " << res->isFirst() << endl;
    if (!res->isBeforeFirst()) {
        throw runtime_error("Cursor should be positioned before the first row");
    }
    /* Move the cursor forward again to position 1 - the first row */
    res->next();
    cout << "#\t Positioning cursor to 1 using next(), isFirst() = " << res->isFirst() << endl;
    validateRow(res, min);
    /* Move the cursor to position 0 = before the first row */
    if (false != res->absolute(0)) {
        throw runtime_error("Call did not fail although its not allowed to move the cursor before the first row");
    }
    cout << "#\t Positioning before first row using absolute(0), isFirst() = " << res->isFirst() << endl;
    /* Move the cursor forward to position 1 = the first row */
    res->next();
    validateRow(res, min);

    /* Move the cursor to position 0 = before the first row */
    res->beforeFirst();
    cout << "#\t Positioning cursor using beforeFirst(), isFirst() = " << res->isFirst() << endl;
    /* Move the cursor forward to position 1 = the first row */
    res->next();
    cout << "#\t\t Moving cursor forward using next(), isFirst() = " << res->isFirst() << endl;
    validateRow(res, min);

    cout << "#\t Finally, reading in descending (reverse) order again" << endl;
    /* Move the cursor after the last row - n + 1 */
    res->afterLast();
    row = res->rowsCount() - 1;
    /* Move the cursor backwards to: n, n - 1, ... 1, 0. Return true if rows are available.*/
    while (res->previous()) {
        cout << "#\t\t Row " << row << ", getRow() " << res->getRow();
        cout << " id = " << res->getInt("id");
        cout << ", label = '" << res->getString("label") << "'" << endl;
        row--;
    }
    /*
    The last call to res->previous() has moved the cursor before the first row
    Cursor position is 0, recall: rows are from 1 ... n
    */
    cout << "#\t\t isBeforeFirst() = " << res->isBeforeFirst() << endl;
    if (true != res->isBeforeFirst()) {
        throw runtime_error("Position should be 0 = before first row");
    }

    cout << "#\t And in regular order..." << endl;
    res->beforeFirst();
    if (true != res->isBeforeFirst()) {
        throw runtime_error("Cursor should be positioned before the first row");
    }
    row = 0;
    while (res->next()) {
        cout << "#\t\t Row " << row << ", getRow() " << res->getRow();
        cout << " id = " << res->getInt("id");
        cout << ", label = '" << res->getString("label") << "'" << endl;
        row++;
    }
    cout << "#\t\t isAfterLast() = " << res->isAfterLast() << endl;
    if (true != res->isAfterLast()) {
        throw runtime_error("next() has returned false and the cursor should be after the last row");
    }
    /* Move to the last entry using a negative offset for absolute() */
    cout << "#\t Trying absolute(-1) to fetch last entry..." << endl;
    if (true != res->absolute(-1)) {
        throw runtime_error("Call did fail although -1 is valid");
    }
    cout << "#\t\t isAfterLast() = " << res->isAfterLast() << endl;
    if (false != res->isAfterLast()) {
        throw runtime_error("Cursor should be positioned to the last row and not after the last row");
    }
    cout << "#\t\t isLast() = " << res->isLast() << endl;
    if (true != res->isLast()) {
        throw runtime_error("Cursor should be positioned to the last row");
    }
    validateRow(res, max);
    /* Another way to move after the last entry */
    cout << "#\t Trying absolute(NUMROWS + 10) to move cursor after last row and fetch last entry..." << endl;
    if (false != res->absolute(res->rowsCount() + 10)) {
        throw runtime_error("Call did fail although parameter is valid");
    }
    if (true != res->isAfterLast()) {
        throw runtime_error("Cursor should be positioned after the last row");
    }
    cout << "#\t\t isLast() = " << res->isLast() << endl;
    if (false != res->isLast()) {
        throw runtime_error("Cursor should be positioned after the last row");
    }
    try {
        res->getString(1);
        throw runtime_error("Fetching is possible although cursor is out of range");
    } catch (sql::InvalidArgumentException &) {
        cout << "#\t\t OK, fetching not allowed when cursor is out of range..." << endl;
    }
    /* absolute(NUM_ROWS + 10) is internally aligned to NUM_ROWS + 1 = afterLastRow() */
    res->previous();
    validateRow(res, max);
}