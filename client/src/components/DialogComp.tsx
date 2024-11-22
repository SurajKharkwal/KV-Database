import { Button } from "@/components/ui/button";
import {
  Dialog,
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle,
  DialogTrigger,
} from "@/components/ui/dialog";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Plus } from "lucide-react";
import { useState } from "react";

export function CustomDialog({
  type,
  data,
}: {
  type: "Update" | "Add";
  data?: { key: string; value: string };
}) {
  const [key, setKey] = useState(data?.key || "");
  const [value, setValue] = useState(data?.value || "");
  const [loading, setLoading] = useState(false); // State to manage button disablement

  const handleSubmit = async () => {
    setLoading(true); // Disable the button
    try {
      if (type === "Update") {
        const res = await fetch("/updateKv", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ key, value }),
        });
        if (!res.ok) throw new Error("Failed to update KV pair");
        console.log("KV pair updated successfully");
      } else if (type === "Add") {
        const res = await fetch("/setKv", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ key, value }),
        });
        if (!res.ok) throw new Error("Failed to add KV pair");
        console.log("KV pair added successfully");
      }
    } catch (error) {
      console.error("Error:", error);
    } finally {
      setLoading(false); // Re-enable the button
    }
  };

  return (
    <Dialog>
      <DialogTrigger asChild>
        <Button variant={type === "Update" ? "link" : "default"}>
          {type === "Add" ? (
            <>
              <Plus /> Add
            </>
          ) : (
            type
          )}
        </Button>
      </DialogTrigger>
      <DialogContent className="sm:max-w-[425px]">
        <DialogHeader>
          <DialogTitle>{type === "Add" ? "Add KV" : "Update KV"}</DialogTitle>
          <DialogDescription>
            Basic information to make changes to the KV store
          </DialogDescription>
        </DialogHeader>
        <div className="grid gap-4 py-4">
          <div className="grid grid-cols-4 items-center gap-4">
            <Label htmlFor="key" className="text-right">
              Key
            </Label>
            <Input
              id="key"
              placeholder="Enter the key"
              value={key}
              onChange={(e) => setKey(e.target.value)}
              className="col-span-3"
              disabled={type === "Update"} // Disable editing key in update mode
            />
          </div>
          <div className="grid grid-cols-4 items-center gap-4">
            <Label htmlFor="value" className="text-right">
              Value
            </Label>
            <Input
              id="value"
              placeholder="Enter the value"
              value={value}
              onChange={(e) => setValue(e.target.value)}
              className="col-span-3"
            />
          </div>
        </div>
        <DialogFooter>
          <Button type="button" onClick={handleSubmit} disabled={loading}>
            {loading ? "Saving..." : "Save changes"}
          </Button>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  );
}

