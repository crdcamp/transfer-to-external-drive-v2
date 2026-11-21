First thing you want to do here is consider everything you have written down in the comments before going in to the exact plan

Revisit the docs to determine exactly what fts_open and fts_children do. Keep revisiting them until they're hammered in your head.

You need to come up with a concise logical plan for determining **if a file is in the source, but not in the target**, or **if a file is in the target, but not in the source**. A file can only meet one of those two criteria. You'll want to make a directory structure for testing this.

You also might want to consider converting the logic for "if it's in source, but not in target" and "if it's in target, but not in source" into  function to make the coding a bit easier.

A metadata array of both children linked lists could make this logic easier to implement, but that will add overhead that you could avoid if you **really** think through how to do this, since it requires walking the children linked list upon its creation. Since you're passing strings around and not opening and closing file paths, it wouldn't add that much to the actual file processing.

With all those ideas out of the way, this is the main condition your logic needs to handle. If this happens in your directory structure, how do you walk the linked lists to handle this? Don't just start typing a think through, **actually think** about it. There is certainly a way to compare these lists without walking them beforehand. After all, they're already sorted!
 * **Consider doing various internet searches for comparing sorted linked lists**.
    * Update: The only results I found were very basic and cannot be used to answer this question.
 * **Start by creating a very simple test linked list like the one below**. You don't have to go straight to the actual implementation to understand the basis of how it'd work.
 * **Another thing worth considering:** There are no duplicates in this problem. No OS will allow a duplicate file! SO, that actually makes the problem a bit easier!

```
source: a  b  c
target: a  c
```
