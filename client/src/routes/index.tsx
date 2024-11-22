"use client";

import { createFileRoute } from "@tanstack/react-router";
import * as React from "react";
import {
  ColumnDef,
  ColumnFiltersState,
  SortingState,
  flexRender,
  getCoreRowModel,
  getFilteredRowModel,
  getPaginationRowModel,
  getSortedRowModel,
  useReactTable,
} from "@tanstack/react-table";
import { ArrowUpDown, MoreHorizontal } from "lucide-react";

import { Button } from "@/components/ui/button";
import {
  DropdownMenu,
  DropdownMenuContent,
  DropdownMenuItem,
  DropdownMenuLabel,
  DropdownMenuSeparator,
  DropdownMenuTrigger,
} from "@/components/ui/dropdown-menu";
import { Input } from "@/components/ui/input";
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow,
} from "@/components/ui/table";
import { CustomDialog } from "@/components/DialogComp";

export const Route = createFileRoute("/")({
  component: DataTableDemo,
});

export type KvStore = {
  key: string;
  value: string;
};

export const columns: ColumnDef<KvStore>[] = [
  {
    accessorKey: "key",
    header: "Key",
    cell: ({ row }) => <div className="capitalize">{row.getValue("key")}</div>,
  },
  {
    accessorKey: "value",
    header: ({ column }) => (
      <Button
        variant="ghost"
        onClick={() => column.toggleSorting(column.getIsSorted() === "asc")}
      >
        Value
        <ArrowUpDown />
      </Button>
    ),
    cell: ({ row }) => <div className="lowercase">{row.getValue("value")}</div>,
  },
  {
    id: "actions",
    enableHiding: false,
    cell: ({ row }) => {
      const kv = row.original;
        const handleDelete = async (kv: KvStore) => {
  try {
    const res = await fetch("/deleteKv", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({ key: kv.key }),
    });

    if (!res.ok) {
      throw new Error("Failed to delete the key");
    }

    console.log("Delete successful:", res.text());

    // Optionally update the UI after deletion
  } catch (error) {
    console.error("Error deleting the key:", error);
  }
};
      return (
        <DropdownMenu>
          <DropdownMenuTrigger asChild>
            <Button variant="ghost" className="h-8 w-8 p-0">
              <span className="sr-only">Open menu</span>
              <MoreHorizontal />
            </Button>
          </DropdownMenuTrigger>
          <DropdownMenuContent align="end">
            <DropdownMenuLabel>Actions</DropdownMenuLabel>
            <DropdownMenuItem onClick={()=>handleDelete(kv)}>Delete</DropdownMenuItem>
            <DropdownMenuSeparator />
            <CustomDialog type="Update" data={kv} />
          </DropdownMenuContent>
        </DropdownMenu>
      );
    },
  },
];

export function DataTableDemo() {
  const [sorting, setSorting] = React.useState<SortingState>([]);
  const [columnFilters, setColumnFilters] = React.useState<ColumnFiltersState>(
    []
  );
  const [data, setData] = React.useState<KvStore[]>([]);
  const [loading, setLoading] = React.useState(true);

  React.useEffect(() => {
  const fetchData = async () => {
    try {
      const res = await fetch("/api/getAll", { method: "GET" });
      if (!res.ok) {
        throw new Error("Failed to fetch data");
      }

      const textData = await res.text();
      console.log(textData); // Check the raw response

      // Parse the text data to extract key-value pairs
      const parsedData = textData
        .split("\n") // Split into lines
        .filter(line => line.includes(":")) // Filter out lines without ":"
        .map(line => {
          const [key, value] = line.split(":").map(str => str.trim()); // Split and trim
          return { key, value };
        });

      setData(parsedData.slice(1,)); // Set the processed data
    } catch (error) {
      console.error("Error fetching data:", error);
    } finally {
      setLoading(false);
    }
  };

  fetchData();
}, []);


  const table = useReactTable({
    data,
    columns,
    onSortingChange: setSorting,
    onColumnFiltersChange: setColumnFilters,
    getCoreRowModel: getCoreRowModel(),
    getPaginationRowModel: getPaginationRowModel(),
    getSortedRowModel: getSortedRowModel(),
    getFilteredRowModel: getFilteredRowModel(),
    state: {
      sorting,
      columnFilters,
    },
  });

  return (
    <div className="w-[100dvw] h-[100dvh] flex flex-col items-center justify-center">
      <h1 className="font-bold text-3xl">KV Store</h1>
      <div className="p-16 shadow-md rounded-md">
        {loading ? (
          <p>Loading...</p>
        ) : (
          <div className="w-[500px]">
            <div className="flex items-center py-4">
              <Input
                placeholder="Filter values..."
                value={(table.getColumn("value")?.getFilterValue() as string) ?? ""}
                onChange={(event) =>
                  table.getColumn("value")?.setFilterValue(event.target.value)
                }
                className="max-w-sm mr-auto"
              />
              <CustomDialog type="Add" />
            </div>

            <div className="rounded-md border">
              <Table>
                <TableHeader>
                  {table.getHeaderGroups().map((headerGroup) => (
                    <TableRow key={headerGroup.id}>
                      {headerGroup.headers.map((header) => (
                        <TableHead key={header.id}>
                          {header.isPlaceholder
                            ? null
                            : flexRender(
                                header.column.columnDef.header,
                                header.getContext()
                              )}
                        </TableHead>
                      ))}
                    </TableRow>
                  ))}
                </TableHeader>
                <TableBody>
                  {table.getRowModel().rows?.length ? (
                    table.getRowModel().rows.map((row) => (
                      <TableRow key={row.id}>
                        {row.getVisibleCells().map((cell) => (
                          <TableCell key={cell.id}>
                            {flexRender(
                              cell.column.columnDef.cell,
                              cell.getContext()
                            )}
                          </TableCell>
                        ))}
                      </TableRow>
                    ))
                  ) : (
                    <TableRow>
                      <TableCell
                        colSpan={columns.length}
                        className="h-24 text-center"
                      >
                        No results.
                      </TableCell>
                    </TableRow>
                  )}
                </TableBody>
              </Table>
            </div>
            <div className="flex items-center justify-end space-x-2 py-4">
              <Button
                variant="outline"
                size="sm"
                onClick={() => table.previousPage()}
                disabled={!table.getCanPreviousPage()}
              >
                Previous
              </Button>
              <Button
                variant="outline"
                size="sm"
                onClick={() => table.nextPage()}
                disabled={!table.getCanNextPage()}
              >
                Next
              </Button>
            </div>
          </div>
        )}
      </div>
    </div>
  );
}

