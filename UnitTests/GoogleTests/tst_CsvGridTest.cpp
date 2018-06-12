#ifndef DISABLE_CSVGRID_TESTS

#include "CsvGrid/CsvGrid.h"
#include <limits>
#include <fstream>
#include <string>
#include <boost/predef.h>
#include <boost/filesystem.hpp>

#include "gtest/gtest.h"

using namespace core_lib::csv_grid;
namespace bfs = boost::filesystem;

#if BOOST_OS_LINUX
static const std::string path1 = "../data/testfile1.csv";
static const std::string path2 = "../data/testfile2.csv";
#else
static const std::string path1 = "../../../data/testfile1.csv";
static const std::string path2 = "../../../data/testfile2.csv";
#endif

TEST(CsvGridTest, Cell_DefaultConstructor)
{
    Cell        cell;
    std::string value = cell;
    EXPECT_STREQ(value.c_str(), "");
}

TEST(CsvGridTest, Cell_StringConstructor)
{
    Cell        cell("test string");
    std::string value = cell;
    EXPECT_STREQ(value.c_str(), "test string");
}

TEST(CsvGridTest, Cell_CopyConstructor)
{
    Cell        cell("test string");
    Cell        cellCopy(cell);
    std::string value     = cell;
    std::string valueCopy = cellCopy;
    EXPECT_STREQ(value.c_str(), valueCopy.c_str());
}

TEST(CsvGridTest, Cell_MoveConstructor)
{
    Cell        cell(Cell("test string"));
    std::string value = cell;
    EXPECT_STREQ(value.c_str(), "test string");
}

TEST(CsvGridTest, Cell_Int32Constructor)
{
    int32_t value1 = std::numeric_limits<int32_t>::max() / 2;
    Cell    cell(value1);
    int32_t value2 = cell;
    EXPECT_EQ(value1, value2);
}

TEST(CsvGridTest, Cell_Int64Constructor)
{
    int64_t value1 = std::numeric_limits<int64_t>::max() / 2;
    Cell    cell(value1);
    int64_t value2 = cell;
    EXPECT_EQ(value1, value2);
}

TEST(CsvGridTest, Cell_DoubleConstructor)
{
    double value1 = std::numeric_limits<double>::max() / 2;
    Cell   cell(value1);
    double value2 = cell;
    EXPECT_DOUBLE_EQ(value1, value2);
}

TEST(CsvGridTest, Cell_CopyAssignment)
{
    Cell cell("test string");
    Cell cellCopy;
    cellCopy              = cell;
    std::string value     = cell;
    std::string valueCopy = cellCopy;
    EXPECT_STREQ(value.c_str(), valueCopy.c_str());
}

TEST(CsvGridTest, Cell_MoveAssignment)
{
    Cell cell;
    cell              = Cell("test string");
    std::string value = cell;
    EXPECT_STREQ(value.c_str(), "test string");
}

TEST(CsvGridTest, Cell_StringAssignment)
{
    Cell        cell;
    std::string value     = "test string";
    cell                  = value;
    std::string cellValue = cell;
    EXPECT_STREQ(cellValue.c_str(), value.c_str());
}

TEST(CsvGridTest, Cell_Int32Assignment)
{
    Cell    cell;
    int32_t value1 = std::numeric_limits<int32_t>::max() / 2;
    cell           = value1;
    int32_t value2 = cell;
    EXPECT_EQ(value1, value2);
}

TEST(CsvGridTest, Cell_Int64Assignment)
{
    Cell    cell;
    int64_t value1 = std::numeric_limits<int64_t>::max() / 2;
    cell           = value1;
    int64_t value2 = cell;
    EXPECT_EQ(value1, value2);
}

TEST(CsvGridTest, Cell_DoubleAssignment)
{
    Cell   cell;
    double value1 = std::numeric_limits<double>::max() / 2;
    cell          = value1;
    double value2 = cell;
    EXPECT_DOUBLE_EQ(value1, value2);
}

TEST(CsvGridTest, Cell_Int32ConvertException)
{
    bool correctException;

    try
    {
        Cell    cell("I'm not a number I'm a free man!");
        int32_t temp = static_cast<int32_t>(cell);
        (void)temp;
        correctException = false;
    }
    catch (std::invalid_argument& e)
    {
        (void)e;
        correctException = true;
    }
    catch (...)
    {
        correctException = false;
    }

    EXPECT_TRUE(correctException);
}

TEST(CsvGridTest, Cell_Int64ConvertException)
{
    bool correctException;

    try
    {
        Cell    cell("I'm not a number I'm a free man!");
        int64_t temp = static_cast<int64_t>(cell);
        (void)temp;
        correctException = false;
    }
    catch (std::invalid_argument& e)
    {
        (void)e;
        correctException = true;
    }
    catch (...)
    {
        correctException = false;
    }

    EXPECT_TRUE(correctException);
}

TEST(CsvGridTest, Cell_DoubleConvertException)
{
    bool correctException;

    try
    {
        Cell   cell("I'm not a number I'm a free man!");
        double temp = static_cast<double>(cell);
        (void)temp;
        correctException = false;
    }
    catch (std::invalid_argument& e)
    {
        (void)e;
        correctException = true;
    }
    catch (...)
    {
        correctException = false;
    }

    EXPECT_TRUE(correctException);
}

TEST(CsvGridTest, Cell_ToInt32Def_1)
{
    Cell          cell("I'm not a number I'm a free man!");
    const int32_t testValue = std::numeric_limits<int32_t>::max() / 2;
    int32_t       value     = cell.ToInt32Def(testValue);
    EXPECT_EQ(value, testValue);
}

TEST(CsvGridTest, Cell_ToInt32Def_2)
{
    const int32_t testValue = std::numeric_limits<int32_t>::max() / 2;
    Cell          cell(testValue);
    int32_t       value = cell.ToInt32Def(0);
    EXPECT_EQ(value, testValue);
}

TEST(CsvGridTest, Cell_ToInt64Def_1)
{
    Cell          cell("I'm not a number I'm a free man!");
    const int64_t testValue = std::numeric_limits<int64_t>::max() / 2;
    int64_t       value     = cell.ToInt64Def(testValue);
    EXPECT_EQ(value, testValue);
}

TEST(CsvGridTest, Cell_ToInt64Def_2)
{
    const int64_t testValue = std::numeric_limits<int64_t>::max() / 2;
    Cell          cell(testValue);
    int64_t       value = cell.ToInt64Def(0);
    EXPECT_EQ(value, testValue);
}

TEST(CsvGridTest, Cell_ToDoubleDef_1)
{
    Cell         cell("I'm not a number I'm a free man!");
    const double testValue = std::numeric_limits<double>::max() / 2;
    double       value     = cell.ToDoubleDef(testValue);
    EXPECT_EQ(value, testValue);
}

TEST(CsvGridTest, Cell_ToDoubleDef_2)
{
    const double testValue = std::numeric_limits<double>::max() / 2;
    Cell         cell(testValue);
    double       value = cell.ToDoubleDef(0);
    EXPECT_DOUBLE_EQ(value, testValue);
}

TEST(CsvGridTest, Row_DefaultConstructor)
{
    Row row;
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(0));
}

TEST(CsvGridTest, Row_CopyConstructor)
{
    Row row;
    row.SetSize(100);
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(100));
    Row rowCopy(row);
    EXPECT_EQ(row.GetSize(), rowCopy.GetSize());
}

TEST(CsvGridTest, Row_MoveConstructor)
{
    Row row(Row(100));
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(100));
}

TEST(CsvGridTest, Row_InitializingConstructor)
{
    Row row(666);
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(666));
}

TEST(CsvGridTest, Row_InitializerListCellConstructor)
{
    Row row = {Cell(), Cell(), Cell(), Cell(), Cell()};
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(5));
}

TEST(CsvGridTest, Row_InitializerListStringConstructor)
{
    Row row = {Cell(""), Cell(""), Cell(""), Cell(""), Cell("")};
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(5));
}

TEST(CsvGridTest, Row_InitializerListInt32Constructor)
{
    int32_t i   = 0;
    Row     row = {Cell(i), Cell(i), Cell(i), Cell(i), Cell(i)};
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(5));
}

TEST(CsvGridTest, Row_InitializerListInt64Constructor)
{
    int64_t i   = 0;
    Row     row = {Cell(i), Cell(i), Cell(i), Cell(i), Cell(i)};
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(5));
}

TEST(CsvGridTest, Row_InitializerListDoubleConstructor)
{
    double i   = 0;
    Row    row = {Cell(i), Cell(i), Cell(i), Cell(i), Cell(i)};
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(5));
}

TEST(CsvGridTest, Row_CopyAssignment)
{
    Row row = {Cell(1), Cell(2), Cell(3), Cell(4), Cell(5)};
    Row rowCopy;
    rowCopy = row;
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(5));
}

TEST(CsvGridTest, Row_MoveAssignment)
{
    Row row;
    row = Row(100);
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(100));
}

TEST(CsvGridTest, Row_SubscriptOperator)
{
    Row row = {Cell(1), Cell(2), Cell(3), Cell(4), Cell(5)};
    EXPECT_EQ(static_cast<int32_t>(row[0]), 1);
    EXPECT_EQ(static_cast<int32_t>(row[1]), 2);
    EXPECT_EQ(static_cast<int32_t>(row[2]), 3);
    EXPECT_EQ(static_cast<int32_t>(row[3]), 4);
    EXPECT_EQ(static_cast<int32_t>(row[4]), 5);

    bool exceptionThrown;

    try
    {
        int32_t temp = static_cast<int32_t>(row[6]);
        (void)temp;
        exceptionThrown = false;
    }
    catch (...)
    {
        exceptionThrown = true;
    }

    EXPECT_TRUE(exceptionThrown);
}

TEST(CsvGridTest, Row_AddColumnAsString)
{
    Row row;
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(0));
    row.AddColumn("new entry");
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(1));
    EXPECT_EQ(static_cast<std::string>(row[0]), std::string("new entry"));
}

TEST(CsvGridTest, Row_AddColumnAsInt32)
{
    Row row;
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(0));
    row.AddColumn(int32_t(100));
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(1));
    EXPECT_EQ(static_cast<int32_t>(row[0]), int32_t(100));
}

TEST(CsvGridTest, Row_AddColumnAsInt64)
{
    Row row;
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(0));
    row.AddColumn(int64_t(100));
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(1));
    EXPECT_EQ(static_cast<int64_t>(row[0]), int64_t(100));
}

TEST(CsvGridTest, Row_AddColumnAsDouble)
{
    Row row;
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(0));
    row.AddColumn(double(100.1));
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(1));
    EXPECT_EQ(static_cast<double>(row[0]), double(100.1));
}

TEST(CsvGridTest, Row_InsertColumnAsString)
{
    Row row = {Cell("1"), Cell("2"), Cell("3"), Cell("4"), Cell("5")};
    row.InsertColumn(3, "666");
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(6));
    EXPECT_EQ(static_cast<std::string>(row[0]), std::string("1"));
    EXPECT_EQ(static_cast<std::string>(row[1]), std::string("2"));
    EXPECT_EQ(static_cast<std::string>(row[2]), std::string("3"));
    EXPECT_EQ(static_cast<std::string>(row[3]), std::string("666"));
    EXPECT_EQ(static_cast<std::string>(row[4]), std::string("4"));
    EXPECT_EQ(static_cast<std::string>(row[5]), std::string("5"));

    bool exceptionThrown;

    try
    {
        row.InsertColumn(100, "667");
        exceptionThrown = false;
    }
    catch (std::out_of_range& e)
    {
        (void)e;
        exceptionThrown = true;
    }
    catch (...)
    {
        exceptionThrown = false;
    }

    EXPECT_TRUE(exceptionThrown);
}

TEST(CsvGridTest, Row_InsertColumnAsInt32)
{
    Row row = {Cell(1), Cell(2), Cell(3), Cell(4), Cell(5)};
    row.InsertColumn(3, static_cast<int32_t>(666));
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(6));
    EXPECT_EQ(static_cast<int32_t>(row[0]), int32_t(1));
    EXPECT_EQ(static_cast<int32_t>(row[1]), int32_t(2));
    EXPECT_EQ(static_cast<int32_t>(row[2]), int32_t(3));
    EXPECT_EQ(static_cast<int32_t>(row[3]), int32_t(666));
    EXPECT_EQ(static_cast<int32_t>(row[4]), int32_t(4));
    EXPECT_EQ(static_cast<int32_t>(row[5]), int32_t(5));
}

TEST(CsvGridTest, Row_InsertColumnAsInt64)
{
    Row row = {Cell(1), Cell(2), Cell(3), Cell(4), Cell(5)};
    row.InsertColumn(3, static_cast<int64_t>(666));
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(6));
    EXPECT_EQ(static_cast<int64_t>(row[0]), int64_t(1));
    EXPECT_EQ(static_cast<int64_t>(row[1]), int64_t(2));
    EXPECT_EQ(static_cast<int64_t>(row[2]), int64_t(3));
    EXPECT_EQ(static_cast<int64_t>(row[3]), int64_t(666));
    EXPECT_EQ(static_cast<int64_t>(row[4]), int64_t(4));
    EXPECT_EQ(static_cast<int64_t>(row[5]), int64_t(5));
}

TEST(CsvGridTest, Row_InsertColumnAsDouble)
{
    Row row = {Cell(1.1), Cell(2.2), Cell(3.3), Cell(4.4), Cell(5.5)};
    row.InsertColumn(3, static_cast<double>(666.6));
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(6));
    EXPECT_EQ(static_cast<double>(row[0]), double(1.1));
    EXPECT_EQ(static_cast<double>(row[1]), double(2.2));
    EXPECT_EQ(static_cast<double>(row[2]), double(3.3));
    EXPECT_EQ(static_cast<double>(row[3]), double(666.6));
    EXPECT_EQ(static_cast<double>(row[4]), double(4.4));
    EXPECT_EQ(static_cast<double>(row[5]), double(5.5));
}

TEST(CsvGridTest, Row_ClearCells)
{
    Row row = {Cell(1), Cell(2), Cell(3), Cell(4), Cell(5)};
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(5));
    row.ClearCells();
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(5));
    EXPECT_EQ(static_cast<std::string>(row[0]), std::string(""));
    EXPECT_EQ(static_cast<std::string>(row[1]), std::string(""));
    EXPECT_EQ(static_cast<std::string>(row[2]), std::string(""));
    EXPECT_EQ(static_cast<std::string>(row[3]), std::string(""));
    EXPECT_EQ(static_cast<std::string>(row[4]), std::string(""));
}

TEST(CsvGridTest, Row_ResetRow)
{
    Row row = {Cell(1), Cell(2), Cell(3), Cell(4), Cell(5)};
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(5));
    row.ResetRow();
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(0));
}

TEST(CsvGridTest, CsvGrid_DefaultConstructor)
{
    CsvGrid grid;
    EXPECT_EQ(grid.GetRowCount(), static_cast<size_t>(0));
}

TEST(CsvGridTest, CsvGrid_InitializingConstructor_1)
{
    CsvGrid grid(10, 10);
    EXPECT_EQ(grid.GetRowCount(), static_cast<size_t>(10));

    for (size_t row = 0; row < grid.GetRowCount(); ++row)
    {
        EXPECT_EQ(grid.GetColCount(row), static_cast<size_t>(10));
        EXPECT_EQ(grid[row].GetSize(), static_cast<size_t>(10));
    }
}

TEST(CsvGridTest, CsvGrid_InitializingConstructor_2)
{
    bool exceptionThrown;

    try
    {
        CsvGrid grid(0, 0);
        exceptionThrown = false;
    }
    catch (std::out_of_range& e)
    {
        (void)e;
        exceptionThrown = true;
    }
    catch (...)
    {
        exceptionThrown = false;
    }

    EXPECT_TRUE(exceptionThrown);
}

TEST(CsvGridTest, CsvGrid_InitializerListConstructor)
{
    Row     row  = {Cell(1), Cell(2), Cell(3), Cell(4), Cell(5)};
    CsvGrid grid = {row, row, row, row, row};

    EXPECT_EQ(grid.GetRowCount(), static_cast<size_t>(5));

    for (size_t r = 0; r < grid.GetRowCount(); ++r)
    {
        EXPECT_EQ(grid.GetColCount(r), static_cast<size_t>(5));
        EXPECT_EQ(grid[r].GetSize(), static_cast<size_t>(5));
    }
}

TEST(CsvGridTest, CsvGrid_CopyConstructor)
{
    Row     row  = {Cell(1), Cell(2), Cell(3), Cell(4), Cell(5)};
    CsvGrid grid = {row, row, row, row, row};
    CsvGrid gridCopy(grid);

    EXPECT_EQ(gridCopy.GetRowCount(), static_cast<size_t>(5));

    for (size_t r = 0; r < gridCopy.GetRowCount(); ++r)
    {
        EXPECT_EQ(gridCopy.GetColCount(r), static_cast<size_t>(5));
        EXPECT_EQ(gridCopy[r].GetSize(), static_cast<size_t>(5));
    }
}

TEST(CsvGridTest, CsvGrid_MoveConstructor)
{
    Row     row = {Cell(1), Cell(2), Cell(3), Cell(4), Cell(5)};
    CsvGrid gridCopy(CsvGrid{row, row, row, row, row});

    EXPECT_EQ(gridCopy.GetRowCount(), static_cast<size_t>(5));

    for (size_t r = 0; r < gridCopy.GetRowCount(); ++r)
    {
        EXPECT_EQ(gridCopy.GetColCount(r), static_cast<size_t>(5));
        EXPECT_EQ(gridCopy[r].GetSize(), static_cast<size_t>(5));
    }
}

TEST(CsvGridTest, CsvGrid_FileConstructor_SimpleCells)
{
    try
    {
        CsvGrid grid(path1, eCellFormatOptions::simpleCells);
        EXPECT_EQ(grid.GetRowCount(), static_cast<size_t>(1000));
        EXPECT_EQ(grid[0].GetSize(), static_cast<size_t>(130));
        EXPECT_EQ(grid[999].GetSize(), static_cast<size_t>(130));
        EXPECT_EQ(static_cast<std::string>(grid[0][0]), std::string("I am a test string"));
        EXPECT_EQ(static_cast<std::string>(grid[999][129]), std::string("I am a test string"));
    }
    catch (...)
    {
        FAIL() << "unexpected exception caught when loading csv file";
    }
}

TEST(CsvGridTest, CsvGrid_FileConstructor_SimpleCells_Benchmark_1000by130Cells)
{
    try
    {
        CsvGrid grid(path1, eCellFormatOptions::simpleCells);
    }
    catch (...)
    {
        FAIL() << "unexpected exception caught when loading csv file";
    }
}

TEST(CsvGridTest, CsvGrid_FileConstructor_DoubleQuotedCells)
{
    try
    {
        CsvGrid grid(path2, eCellFormatOptions::doubleQuotedCells);
        EXPECT_EQ(grid.GetRowCount(), static_cast<size_t>(1000));
        EXPECT_EQ(grid[0].GetSize(), static_cast<size_t>(130));
        EXPECT_EQ(grid[999].GetSize(), static_cast<size_t>(130));
        EXPECT_EQ(static_cast<std::string>(grid[0][0]), std::string("I am a test, string"));
        EXPECT_EQ(static_cast<std::string>(grid[999][129]), std::string("I am a test, string"));
    }
    catch (...)
    {
        FAIL() << "unexpected exception caught when loading csv file";
    }
}

TEST(CsvGridTest, CsvGrid_FileConstructor_DoubleQuotedCells_Benchmark_1000by130Cells)
{
    try
    {
        CsvGrid grid(path2, eCellFormatOptions::doubleQuotedCells);
    }
    catch (...)
    {
        FAIL() << "unexpected exception caught when loading csv file";
    }
}

TEST(CsvGridTest, CsvGrid_SetColCount)
{
    CsvGrid grid(10, 10);
    EXPECT_EQ(grid.GetRowCount(), static_cast<size_t>(10));

    for (size_t row = 0; row < grid.GetRowCount(); ++row)
    {
        EXPECT_EQ(grid.GetColCount(row), static_cast<size_t>(10));
        EXPECT_EQ(grid[row].GetSize(), static_cast<size_t>(10));
    }

    grid.SetRowCount(15, 5);

    EXPECT_EQ(grid.GetRowCount(), static_cast<size_t>(15));

    for (size_t row = 0; row < grid.GetRowCount(); ++row)
    {
        if (row < 10)
        {
            EXPECT_EQ(grid.GetColCount(row), static_cast<size_t>(10));
            EXPECT_EQ(grid[row].GetSize(), static_cast<size_t>(10));
        }
        else
        {
            EXPECT_EQ(grid.GetColCount(row), static_cast<size_t>(5));
            EXPECT_EQ(grid[row].GetSize(), static_cast<size_t>(5));
        }
    }

    grid.SetRowCount(5, 5);

    EXPECT_EQ(grid.GetRowCount(), static_cast<size_t>(5));

    for (size_t row = 0; row < grid.GetRowCount(); ++row)
    {
        EXPECT_EQ(grid.GetColCount(row), static_cast<size_t>(10));
        EXPECT_EQ(grid[row].GetSize(), static_cast<size_t>(10));
    }
}

TEST(CsvGridTest, CsvGrid_AddRow)
{
    CsvGrid grid(10, 10);
    grid.AddRow(5);

    EXPECT_EQ(grid.GetRowCount(), static_cast<size_t>(11));

    for (size_t row = 0; row < grid.GetRowCount(); ++row)
    {
        if (row < 10)
        {
            EXPECT_EQ(grid.GetColCount(row), static_cast<size_t>(10));
            EXPECT_EQ(grid[row].GetSize(), static_cast<size_t>(10));
        }
        else
        {
            EXPECT_EQ(grid.GetColCount(row), static_cast<size_t>(5));
            EXPECT_EQ(grid[row].GetSize(), static_cast<size_t>(5));
        }
    }
}

TEST(CsvGridTest, CsvGrid_AddColumnToAllRows)
{
    Row     row1 = {Cell(1)};
    Row     row2 = {Cell(1), Cell(2)};
    Row     row3 = {Cell(1), Cell(2), Cell(3)};
    CsvGrid grid = {row1, row2, row3};

    grid.AddColumnToAllRows();

    EXPECT_EQ(grid[0].GetSize(), static_cast<size_t>(2));
    EXPECT_EQ(grid[1].GetSize(), static_cast<size_t>(3));
    EXPECT_EQ(grid[2].GetSize(), static_cast<size_t>(4));
}

TEST(CsvGridTest, CsvGrid_InsertRow)
{
    Row     row1 = {Cell(1)};
    Row     row2 = {Cell(1), Cell(2)};
    Row     row3 = {Cell(1), Cell(2), Cell(3)};
    CsvGrid grid = {row1, row2, row3};

    grid.InsertRow(1, 5);
    grid.InsertRow(2);

    EXPECT_EQ(grid.GetRowCount(), static_cast<size_t>(5));

    EXPECT_EQ(grid.GetColCount(0), static_cast<size_t>(1));
    EXPECT_EQ(grid[0].GetSize(), static_cast<size_t>(1));

    EXPECT_EQ(grid.GetColCount(1), static_cast<size_t>(5));
    EXPECT_EQ(grid[1].GetSize(), static_cast<size_t>(5));

    EXPECT_EQ(grid.GetColCount(2), static_cast<size_t>(0));
    EXPECT_EQ(grid[2].GetSize(), static_cast<size_t>(0));

    EXPECT_EQ(grid.GetColCount(3), static_cast<size_t>(2));
    EXPECT_EQ(grid[3].GetSize(), static_cast<size_t>(2));

    EXPECT_EQ(grid.GetColCount(4), static_cast<size_t>(3));
    EXPECT_EQ(grid[4].GetSize(), static_cast<size_t>(3));

    bool correctException;

    try
    {
        grid.InsertRow(100);
        correctException = false;
    }
    catch (std::out_of_range& e)
    {
        (void)e;
        correctException = true;
    }
    catch (...)
    {
        correctException = false;
    }

    EXPECT_TRUE(correctException);
}

TEST(CsvGridTest, CsvGrid_InsertColumnInAllRows)
{
    Row     row1 = {Cell(1), Cell(2)};
    Row     row2 = {Cell(1), Cell(2), Cell(3)};
    Row     row3 = {Cell(1), Cell(2), Cell(3), Cell(4)};
    CsvGrid grid = {row1, row2, row3};

    grid.InsertColumnInAllRows(1);

    EXPECT_EQ(grid.GetColCount(0), static_cast<size_t>(3));
    EXPECT_EQ(grid[0].GetSize(), static_cast<size_t>(3));

    EXPECT_EQ(grid.GetColCount(1), static_cast<size_t>(4));
    EXPECT_EQ(grid[1].GetSize(), static_cast<size_t>(4));

    EXPECT_EQ(grid.GetColCount(2), static_cast<size_t>(5));
    EXPECT_EQ(grid[2].GetSize(), static_cast<size_t>(5));

    std::string cell = static_cast<std::string>(grid[0][1]);
    EXPECT_STREQ(cell.c_str(), "");

    cell = static_cast<std::string>(grid[1][1]);
    EXPECT_STREQ(cell.c_str(), "");

    cell = static_cast<std::string>(grid[2][1]);
    EXPECT_STREQ(cell.c_str(), "");

    try
    {
        grid.InsertColumnInAllRows(100);

        EXPECT_EQ(grid.GetColCount(0), static_cast<size_t>(3));
        EXPECT_EQ(grid[0].GetSize(), static_cast<size_t>(3));

        EXPECT_EQ(grid.GetColCount(1), static_cast<size_t>(4));
        EXPECT_EQ(grid[1].GetSize(), static_cast<size_t>(4));

        EXPECT_EQ(grid.GetColCount(2), static_cast<size_t>(5));
        EXPECT_EQ(grid[2].GetSize(), static_cast<size_t>(5));
    }
    catch (...)
    {
        FAIL() << "unexpected exception caught when inserting column to all rows";
    }
}

TEST(CsvGridTest, CsvGrid_ClearCells)
{
    Row     row1 = {Cell(1), Cell(2)};
    Row     row2 = {Cell(1), Cell(2), Cell(3)};
    Row     row3 = {Cell(1), Cell(2), Cell(3), Cell(4)};
    CsvGrid grid = {row1, row2, row3};

    grid.ClearCells();

    EXPECT_EQ(grid.GetRowCount(), static_cast<size_t>(3));

    EXPECT_EQ(grid.GetColCount(0), static_cast<size_t>(2));
    EXPECT_EQ(grid[0].GetSize(), static_cast<size_t>(2));

    EXPECT_EQ(grid.GetColCount(1), static_cast<size_t>(3));
    EXPECT_EQ(grid[1].GetSize(), static_cast<size_t>(3));

    EXPECT_EQ(grid.GetColCount(2), static_cast<size_t>(4));
    EXPECT_EQ(grid[2].GetSize(), static_cast<size_t>(4));

    for (size_t row = 0; row < grid.GetRowCount(); ++row)
    {
        for (size_t col = 0; col < grid.GetColCount(row); ++col)
        {
            std::string value = grid[row][col];
            EXPECT_STREQ(value.c_str(), "");
        }
    }
}

TEST(CsvGridTest, CsvGrid_ResetGrid)
{
    Row     row1 = {Cell(1), Cell(2)};
    Row     row2 = {Cell(1), Cell(2), Cell(3)};
    Row     row3 = {Cell(1), Cell(2), Cell(3), Cell(4)};
    CsvGrid grid = {row1, row2, row3};

    grid.ResetGrid();

    EXPECT_EQ(grid.GetRowCount(), static_cast<size_t>(0));
}

TEST(CsvGridTest, CsvGrid_LoadFromCSVFile_1)
{
    CsvGrid grid;
    bool    correctException;

    try
    {
        grid.LoadFromCSVFile("dummyfile.csv", eCellFormatOptions::simpleCells);
        correctException = false;
    }
    catch (std::runtime_error& e)
    {
        (void)e;
        correctException = true;
    }
    catch (...)
    {
        correctException = false;
    }

    EXPECT_TRUE(correctException);
}

TEST(CsvGridTest, CsvGrid_LoadFromCSVFile_2)
{
    CsvGrid grid;
    try
    {
        CsvGrid grid(path1, eCellFormatOptions::simpleCells);
        EXPECT_EQ(grid.GetRowCount(), static_cast<size_t>(1000));
        EXPECT_EQ(grid[0].GetSize(), static_cast<size_t>(130));
        EXPECT_EQ(grid[999].GetSize(), static_cast<size_t>(130));
        EXPECT_EQ(static_cast<std::string>(grid[0][0]), std::string("I am a test string"));
        EXPECT_EQ(static_cast<std::string>(grid[999][129]), std::string("I am a test string"));
    }
    catch (...)
    {
        FAIL() << "unexpected exception caught when loading from file";
    }
}

TEST(CsvGridTest, CsvGrid_LoadFromCSVFile_3)
{
    Row     row1 = {Cell(1), Cell(2)};
    Row     row2 = {Cell(1), Cell(2), Cell(3)};
    Row     row3 = {Cell(1), Cell(2), Cell(3), Cell(4)};
    CsvGrid grid = {row1, row2, row3};

    EXPECT_EQ(grid.GetRowCount(), static_cast<size_t>(3));

    EXPECT_EQ(grid.GetColCount(0), static_cast<size_t>(2));
    EXPECT_EQ(grid[0].GetSize(), static_cast<size_t>(2));

    EXPECT_EQ(grid.GetColCount(1), static_cast<size_t>(3));
    EXPECT_EQ(grid[1].GetSize(), static_cast<size_t>(3));

    EXPECT_EQ(grid.GetColCount(2), static_cast<size_t>(4));
    EXPECT_EQ(grid[2].GetSize(), static_cast<size_t>(4));

    try
    {
        CsvGrid grid(path1, eCellFormatOptions::simpleCells);
        EXPECT_EQ(grid.GetRowCount(), static_cast<size_t>(1000));
        EXPECT_EQ(grid[0].GetSize(), static_cast<size_t>(130));
        EXPECT_EQ(grid[999].GetSize(), static_cast<size_t>(130));
        EXPECT_EQ(static_cast<std::string>(grid[0][0]), std::string("I am a test string"));
        EXPECT_EQ(static_cast<std::string>(grid[999][129]), std::string("I am a test string"));
    }
    catch (...)
    {
        FAIL() << "unexpected exception caught when loading from file";
    }
}

TEST(CsvGridTest, CsvGrid_SaveToCSVFile_1)
{
    Row     row1    = {Cell(1), Cell(2)};
    Row     row2    = {Cell(1), Cell(2), Cell(3)};
    Row     row3    = {Cell(1), Cell(2), Cell(3), Cell(4)};
    CsvGrid gridOut = {row1, row2, row3};

    try
    {
        gridOut.SaveToCsvFile("testSave.csv", eSaveToFileOptions::truncate);
    }
    catch (...)
    {
        FAIL() << "unexpected exception caught when saving to file";
    }

    try
    {
        CsvGrid gridIn("testSave.csv", eCellFormatOptions::simpleCells);

        EXPECT_EQ(gridIn.GetRowCount(), gridOut.GetRowCount());

        for (size_t row = 0; row < gridIn.GetRowCount(); ++row)
        {
            EXPECT_EQ(gridIn.GetColCount(row), gridOut.GetColCount(row));

            for (size_t col = 0; col < gridIn.GetColCount(row); ++col)
            {
                std::string valueIn  = gridIn[row][col];
                std::string valueOut = gridOut[row][col];
                EXPECT_EQ(valueIn, valueOut);
            }
        }

        bfs::remove("testSave.csv");
    }
    catch (...)
    {
        FAIL() << "unexpected exception caught when loading from file";
    }
}

TEST(CsvGridTest, CsvGrid_SaveToCSVFile_2)
{
    Row     row1    = {Cell(1), Cell(2)};
    Row     row2    = {Cell("1,/nbum"), Cell("2"), Cell("3")};
    Row     row3    = {Cell(1), Cell(2), Cell(3), Cell(4)};
    CsvGrid gridOut = {row1, row2, row3};

    try
    {
        gridOut.SaveToCsvFile("testSave.csv", eSaveToFileOptions::truncate);
    }
    catch (...)
    {
        FAIL() << "unexpected exception caught when saving to file";
    }

    try
    {
        CsvGrid gridIn("testSave.csv", eCellFormatOptions::doubleQuotedCells);

        EXPECT_EQ(gridIn.GetRowCount(), gridOut.GetRowCount());

        for (size_t row = 0; row < gridIn.GetRowCount(); ++row)
        {
            EXPECT_EQ(gridIn.GetColCount(row), gridOut.GetColCount(row));

            for (size_t col = 0; col < gridIn.GetColCount(row); ++col)
            {
                std::string valueIn  = gridIn[row][col];
                std::string valueOut = gridOut[row][col];
                EXPECT_EQ(valueIn, valueOut);
            }
        }

        bfs::remove("testSave.csv");
    }
    catch (...)
    {
        FAIL() << "unexpected exception caught when loading from file";
    }
}

TEST(CsvGridTest, Cell_LongDoubleConstructor)
{
    long double value1 = 1000000000.0L;
    Cell        cell(value1);
    long double value2 = cell;
    EXPECT_EQ(value1, value2);
}

TEST(CsvGridTest, Cell_LongDoubleAssignment)
{
    Cell        cell;
    long double value1 = 1000000000.0L;
    cell               = value1;
    long double value2 = cell;
    EXPECT_EQ(value1, value2);
}

TEST(CsvGridTest, Cell_LongDoubleConvertException)
{
    bool correctException;

    try
    {
        Cell        cell("I'm not a number I'm a free man!");
        long double temp = static_cast<long double>(cell);
        (void)temp;
        correctException = false;
    }
    catch (std::invalid_argument& e)
    {
        (void)e;
        correctException = true;
    }
    catch (...)
    {
        correctException = false;
    }

    EXPECT_TRUE(correctException);
}

TEST(CsvGridTest, Cell_LongToDoubleDef_1)
{
    Cell              cell("I'm not a number I'm a free man!");
    const long double testValue = std::numeric_limits<long double>::max() / 2;
    long double       value     = cell.ToLongDoubleDef(testValue);
    EXPECT_EQ(value, testValue);
}

TEST(CsvGridTest, Cell_LongToDoubleDef_2)
{
    const long double testValue = std::numeric_limits<long double>::max() / 2;
    Cell              cell(testValue);
    long double       value = cell.ToLongDoubleDef(0);
    EXPECT_EQ(value, testValue);
}

TEST(CsvGridTest, Row_InitializerListLongDoubleConstructor)
{
    long double i   = 0;
    Row         row = {Cell(i), Cell(i), Cell(i), Cell(i), Cell(i)};
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(5));
}

TEST(CsvGridTest, Row_AddColumnAsLongDouble)
{
    Row row;
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(0));
    row.AddColumn(100.1L);
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(1));
    EXPECT_EQ(static_cast<long double>(row[0]), 100.1L);
}

TEST(CsvGridTest, Row_InsertColumnAsLongDouble)
{
    Row row = {Cell(1.1L), Cell(2.2L), Cell(3.3L), Cell(4.4L), Cell(5.5L)};
    row.InsertColumn(3, 666.6L);
    EXPECT_EQ(row.GetSize(), static_cast<size_t>(6));
    EXPECT_EQ(static_cast<long double>(row[0]), 1.1L);
    EXPECT_EQ(static_cast<long double>(row[1]), 2.2L);
    EXPECT_EQ(static_cast<long double>(row[2]), 3.3L);
    EXPECT_EQ(static_cast<long double>(row[3]), 666.6L);
    EXPECT_EQ(static_cast<long double>(row[4]), 4.4L);
    EXPECT_EQ(static_cast<long double>(row[5]), 5.5L);
}

TEST(CsvGridTest, CellDouble_DefaultConstructor)
{
    CellDouble cell;
    double     value = cell;
    EXPECT_DOUBLE_EQ(value, 0.0);
}

TEST(CsvGridTest, CellDouble_DoubleConstructor)
{
    double     value1 = std::numeric_limits<double>::max() / 2;
    CellDouble cell(value1);
    double     value2 = cell;
    EXPECT_DOUBLE_EQ(value1, value2);
}

TEST(CsvGridTest, CellDouble_CopyConstructor)
{
    CellDouble cell(666.6);
    CellDouble cellCopy(cell);
    double     value     = cell;
    double     valueCopy = cellCopy;
    EXPECT_DOUBLE_EQ(value, valueCopy);
}

TEST(CsvGridTest, CellDouble_MoveConstructor)
{
    CellDouble cell(CellDouble(666.6));
    double     value = cell;
    EXPECT_DOUBLE_EQ(value, 666.6);
}

TEST(CsvGridTest, CellDouble_CopyAssignment)
{
    CellDouble cell(666.6);
    CellDouble cellCopy;
    cellCopy         = cell;
    double value     = cell;
    double valueCopy = cellCopy;
    EXPECT_DOUBLE_EQ(value, valueCopy);
}

TEST(CsvGridTest, CellDouble_MoveAssignment)
{
    CellDouble cell;
    cell         = CellDouble(666.6);
    double value = cell;
    EXPECT_DOUBLE_EQ(value, 666.6);
}

TEST(CsvGridTest, CellDouble_DoubleAssignment_DoubleConversion)
{
    CellDouble cell;
    cell         = 666.6;
    double value = cell;
    EXPECT_DOUBLE_EQ(value, 666.6);
}

TEST(CsvGridTest, CellDouble_StringConversion)
{
    CellDouble  cell(666.6);
    std::string value = cell;
    EXPECT_STREQ(value.c_str(), "666.6");
}

TEST(CsvGridTest, CsvGridD_Specialization)
{
    using namespace core_lib::csv_grid;
    CsvGridD grid{{CellDouble(1.1), CellDouble(2.2), CellDouble(3.3)},
                  {CellDouble(4.4), CellDouble(5.5), CellDouble(6.6)},
                  {CellDouble(7.7), CellDouble(8.8), CellDouble(9.9)}};

    EXPECT_DOUBLE_EQ(grid[0][0], 1.1);
    EXPECT_DOUBLE_EQ(grid[0][1], 2.2);
    EXPECT_DOUBLE_EQ(grid[0][2], 3.3);
    EXPECT_DOUBLE_EQ(grid[1][0], 4.4);
    EXPECT_DOUBLE_EQ(grid[1][1], 5.5);
    EXPECT_DOUBLE_EQ(grid[1][2], 6.6);
    EXPECT_DOUBLE_EQ(grid[2][0], 7.7);
    EXPECT_DOUBLE_EQ(grid[2][1], 8.8);
    EXPECT_DOUBLE_EQ(grid[2][2], 9.9);
}

TEST(CsvGridTest, CsvGridD_SaveLoad)
{
    using namespace core_lib::csv_grid;
    CsvGridD grid{{CellDouble(1.1), CellDouble(2.2), CellDouble(3.3)},
                  {CellDouble(4.4), CellDouble(5.5), CellDouble(6.6)},
                  {CellDouble(7.7), CellDouble(8.8), CellDouble(9.9)}};

    try
    {
        grid.SaveToCsvFile("testSave.csv", eSaveToFileOptions::truncate);
    }
    catch (...)
    {
        FAIL() << "unexpected exception caught when saving to file";
    }

    CsvGridD gridIn;

    try
    {
        gridIn.LoadFromCSVFile("testSave.csv", eCellFormatOptions::simpleCells);
    }
    catch (...)
    {
        FAIL() << "unexpected exception caught when loadig from file";
    }

    EXPECT_DOUBLE_EQ(grid[0][0], gridIn[0][0]);
    EXPECT_DOUBLE_EQ(grid[0][1], gridIn[0][1]);
    EXPECT_DOUBLE_EQ(grid[0][2], gridIn[0][2]);
    EXPECT_DOUBLE_EQ(grid[1][0], gridIn[1][0]);
    EXPECT_DOUBLE_EQ(grid[1][1], gridIn[1][1]);
    EXPECT_DOUBLE_EQ(grid[1][2], gridIn[1][2]);
    EXPECT_DOUBLE_EQ(grid[2][0], gridIn[2][0]);
    EXPECT_DOUBLE_EQ(grid[2][1], gridIn[2][1]);
    EXPECT_DOUBLE_EQ(grid[2][2], gridIn[2][2]);

    bfs::remove("testSave.csv");
}

#endif // DISABLE_CSVGRID_TESTS
