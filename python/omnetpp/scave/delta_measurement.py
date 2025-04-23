"""
Delta measurement functionality for line charts.

This module provides functions to measure the delta (difference) between two data points
on a line chart. It handles finding the nearest data point, drawing/removing selection markers,
calculating and displaying delta values, and key event handling.
"""

import numpy as np
import matplotlib as mpl

# only keep 'ctrl+s', as this conflicts with our delta measurement tool
mpl.rcParams['keymap.save'].remove('s')


# Helper functions for distance calculations
def _distance_squared(x1, y1, x2, y2):
    """Calculates the squared distance between two points."""
    return (x1 - x2) ** 2 + (y1 - y2) ** 2


def _point_segment_distance_squared(px, py, x1, y1, x2, y2):
    """Calculates the squared distance from a point (px, py) to a line segment defined by (x1, y1) and (x2, y2)."""
    segment_len_sq = _distance_squared(x1, y1, x2, y2)
    if segment_len_sq == 0.0:  # segment is a point
        return _distance_squared(px, py, x1, y1)
    t = ((px - x1) * (x2 - x1) + (py - y1) * (y2 - y1)) / segment_len_sq
    t = max(0, min(1, t))
    # the point on the line segment closest to (px, py)
    projX = x1 + t * (x2 - x1)
    projY = y1 + t * (y2 - y1)
    return _distance_squared(px, py, projX, projY)


class DeltaMeasurement:
    """
    Handles delta measurement functionality for line charts.

    This class provides methods to find the nearest data point or segment,
    draw/remove selection markers, calculate and display delta values,
    and handle key events ('a', 's', 'd', 'x').
    """

    PICKING_THRESHOLD_PX = 10

    def __init__(self, fig, axes, xdata=None):
        self.fig = fig
        self.axes = axes
        self.xdata = xdata

        # State variables
        self.endpoint_a = None  # (x, y) tuple for the start point
        self.endpoint_b = None  # (x, y) tuple for the end point
        self.marker_a = None    # Matplotlib artist for marker A
        self.marker_b = None    # Matplotlib artist for marker B

        # Store original format_coord method
        self.original_format_coord = None

        # Connect the key press event handler.
        self.cid = fig.canvas.mpl_connect('key_press_event', self._on_key_press)

    def _find_nearest_point(self, event):
        """
        Find the nearest data point (including interpolated step points) to the mouse cursor.

        Parameters:
        - event: The matplotlib event containing x, y coordinates

        Returns:
        - A tuple (x, y) or None if no point is found within PICKING_THRESHOLD_PX pixels
        """
        if not event.inaxes == self.axes:
            return None

        ax = self.axes
        min_dist_sq = self.PICKING_THRESHOLD_PX ** 2  # Search radius squared
        nearest_point = None

        if self.xdata is not None:

            # Only if xdata is available
            if len(self.xdata) < 1:
                return None

            invTransData = ax.transData.inverted()

            i1 = np.searchsorted(self.xdata, invTransData.transform((event.x - self.PICKING_THRESHOLD_PX, 0.5))[0], "left")
            i2 = np.searchsorted(self.xdata, invTransData.transform((event.x + self.PICKING_THRESHOLD_PX, 0.5))[0], "right")

            # Iterate through data points near cursor
            for i in range(i1, i2):
                x = self.xdata[i]

                # Skip NaN values
                if np.isnan(x):
                    continue

                sx, _ = ax.transData.transform((x, 0.5))
                dist_sq = (sx - event.x) ** 2

                if dist_sq < min_dist_sq:
                    min_dist_sq = dist_sq
                    nearest_point = (x, 0.5)

        else:

            # Check each line in the current axes
            for line in ax.get_lines():
                # Skip lines that are not visible or have no data
                if not line.get_visible() or len(line.get_xdata()) < 1:
                    continue

                # Get line data
                xdata = line.get_xdata()
                ydata = line.get_ydata()
                drawstyle = line.get_drawstyle()

                # Define a helper function to check distance and update nearest point
                def check_point(px, py):
                    nonlocal min_dist_sq, nearest_point
                    # Skip NaN values
                    if np.isnan(px) or np.isnan(py):
                        return False

                    display_coords = ax.transData.transform((px, py))
                    dist_sq = _distance_squared(display_coords[0], display_coords[1],
                                                event.x, event.y)

                    if dist_sq < min_dist_sq:
                        min_dist_sq = dist_sq
                        nearest_point = (px, py)
                        return True
                    return False

                invTransData = ax.transData.inverted()

                i1 = np.searchsorted(xdata, invTransData.transform((event.x - self.PICKING_THRESHOLD_PX, 0.5))[0], "left")
                i2 = np.searchsorted(xdata, invTransData.transform((event.x + self.PICKING_THRESHOLD_PX, 0.5))[0], "right")

                # Iterate through data points near cursor
                for i in range(i1, i2):
                    x = xdata[i]
                    y = ydata[i]

                    # Check the actual data point
                    check_point(x, y)

                    # Check interpolated points for step plots
                    if i > 0:
                        prev_x = xdata[i-1]
                        prev_y = ydata[i-1]
                        # Ensure points are distinct to avoid division by zero or NaN issues later
                        if prev_x != x or prev_y != y:
                            if drawstyle == 'steps-post':
                                # Vertical segment at (x, prev_y) -> (x, y)
                                # Horizontal segment at (prev_x, prev_y) -> (x, prev_y)
                                # Check the corner point (x, prev_y)
                                check_point(x, prev_y)
                            elif drawstyle == 'steps-pre':
                                # Horizontal segment at (prev_x, prev_y) -> (prev_x, y)
                                # Vertical segment at (prev_x, y) -> (x, y)
                                # Check the corner point (prev_x, y)
                                check_point(prev_x, y)
                            # 'default' or 'linear' don't have extra corners to check this way

        return nearest_point

    def _find_nearest_segment(self, event):
        """
        Find the nearest line segment to the mouse cursor, considering draw style.

        Parameters:
        - event: The matplotlib event containing x, y coordinates
        - max_dist: Maximum distance in screen coordinates.

        Returns:
        - A tuple ((x1, y1), (x2, y2)) representing the endpoints of the nearest segment,
          or None if no segment is found within max_dist.
          Note: Endpoints might be interpolated points for step styles.
        """
        if not event.inaxes == self.axes:
            return None

        ax = self.axes
        min_dist_sq = self.PICKING_THRESHOLD_PX ** 2  # Search radius squared
        closest_segment = None

        # Get event position in display coordinates
        sx, sy = event.x, event.y

        if self.xdata is not None:
            n = len(self.xdata)
            invTransData = ax.transData.inverted()

            i1 = np.searchsorted(self.xdata, invTransData.transform((event.x - self.PICKING_THRESHOLD_PX, 0.5))[0], "left")
            i2 = np.searchsorted(self.xdata, invTransData.transform((event.x + self.PICKING_THRESHOLD_PX, 0.5))[0], "right")

            i1 = min(n-1, max(0, i1-1))
            i2 = min(n-1, max(0, i2+1))

            # Iterate through data points near cursor
            for i in range(i1, i2):
                x1_data = self.xdata[i]
                x2_data = self.xdata[i+1]

                # Skip if any coordinate is NaN
                if np.isnan(x1_data) or np.isnan(x2_data):
                    continue

                # Convert data points to screen coordinates for distance check
                sx1, _ = ax.transData.transform((x1_data, 0.5))
                sx2, _ = ax.transData.transform((x2_data, 0.5))

                if sx1 == sx2:  # Segment is a point
                    dist_sq = (sx - sx1) ** 2
                else:
                    l2 = (sx1 - sx2) ** 2
                    t = ((sx - sx1) * (sx2 - sx1)) / l2
                    t = max(0, min(1, t))
                    projX = sx1 + t * (sx2 - sx1)
                    dist_sq = (sx - projX) ** 2

                if dist_sq < min_dist_sq:
                    min_dist_sq = dist_sq
                    closest_segment = (x1_data, 0.5), (x2_data, 0.5)

        else:
            for line in ax.get_lines():

                xdata = line.get_xdata()
                ydata = line.get_ydata()
                n = len(xdata)
                linestyle = line.get_linestyle()
                drawstyle = line.get_drawstyle()

                # Skip lines that are not visible or have no data
                if not line.get_visible() \
                        or n < 2 \
                        or linestyle in [None, 'None', 'none', ' ']:
                    continue

                invTransData = ax.transData.inverted()

                i1 = np.searchsorted(xdata, invTransData.transform((event.x - self.PICKING_THRESHOLD_PX, 0.5))[0], "left")
                i2 = np.searchsorted(xdata, invTransData.transform((event.x + self.PICKING_THRESHOLD_PX, 0.5))[0], "right")

                i1 = min(n-1, max(0, i1-1))
                i2 = min(n-1, max(0, i2+1))

                # Iterate through data points near cursor
                for i in range(i1, i2):
                    x1_data, y1_data = xdata[i], ydata[i]
                    x2_data, y2_data = xdata[i+1], ydata[i+1]

                    # Skip if any coordinate is NaN
                    if np.isnan(x1_data) or np.isnan(y1_data) or np.isnan(x2_data) or np.isnan(y2_data):
                        continue

                    # Convert data points to screen coordinates for distance check
                    sx1, sy1 = ax.transData.transform((x1_data, y1_data))
                    sx2, sy2 = ax.transData.transform((x2_data, y2_data))

                    # Helper to check distance and update closest segment
                    def check_segment(seg_sx1, seg_sy1, seg_sx2, seg_sy2, end1_data, end2_data):
                        nonlocal min_dist_sq, closest_segment
                        dist_sq = _point_segment_distance_squared(sx, sy, seg_sx1, seg_sy1, seg_sx2, seg_sy2)
                        if dist_sq < min_dist_sq:
                            min_dist_sq = dist_sq
                            closest_segment = (end1_data, end2_data)

                    if drawstyle == 'default':
                        # Linear segment: (x1, y1) -> (x2, y2)
                        check_segment(sx1, sy1, sx2, sy2, (x1_data, y1_data), (x2_data, y2_data))
                    elif drawstyle == 'steps-pre':
                        # Vertical segment: (sx1, sy1) -> (sx1, sy2) | Data: (x1, y1) -> (x1, y2)
                        check_segment(sx1, sy1, sx1, sy2, (x1_data, y1_data), (x1_data, y2_data))
                        # Horizontal segment: (sx1, sy2) -> (sx2, sy2) | Data: (x1, y2) -> (x2, y2)
                        check_segment(sx1, sy2, sx2, sy2, (x1_data, y2_data), (x2_data, y2_data))
                    elif drawstyle == 'steps-post':
                        # Horizontal segment: (sx1, sy1) -> (sx2, sy1) | Data: (x1, y1) -> (x2, y1)
                        check_segment(sx1, sy1, sx2, sy1, (x1_data, y1_data), (x2_data, y1_data))
                        # Vertical segment: (sx2, sy1) -> (sx2, sy2) | Data: (x2, y1) -> (x2, y2)
                        check_segment(sx2, sy1, sx2, sy2, (x2_data, y1_data), (x2_data, y2_data))
                    # TODO: 'steps-mid'

        return closest_segment

    def _draw_marker(self, marker, point, symbol):
        """
        Draw or update a marker for an endpoint.
        """

        # Remove existing marker if it exists
        if marker:
            marker.remove()
            self.fig.canvas.draw_idle()

        # Draw new marker if the point is set
        if point:
            line = self.axes.plot(point[0], point[1], marker=symbol, color='red',
                                  ms=8, mfc='none', mew=2, zorder=10,
                                  scalex=False, scaley=False)[0]
            self.fig.canvas.draw_idle()
            return line

    def _draw_marker_a(self):
        """Draw the marker for endpoint A."""
        self.marker_a = self._draw_marker(self.marker_a, self.endpoint_a, 'o')

    def _draw_marker_b(self):
        """Draw the marker for endpoint B."""
        # matplotlib path of 8 one-sixteenth-long arcs around the unit circle
        from matplotlib.path import Path
        marker = Path(
            vertices=[(np.cos(theta), np.sin(theta)) for theta
                      in np.linspace(-0.5, 15.5, 16)/17*2*np.pi],
            codes=([Path.MOVETO, Path.LINETO] * 8)
        )
        self.marker_b = self._draw_marker(self.marker_b, self.endpoint_b, marker)

    def _clear_marker_a(self):
        """Clear the marker for endpoint A."""
        if self.marker_a is not None:
            self.marker_a.remove()
            self.marker_a = None
            self.fig.canvas.draw_idle()

    def _clear_marker_b(self):
        """Clear the marker for endpoint B."""
        if self.marker_b is not None:
            self.marker_b.remove()
            self.marker_b = None
            self.fig.canvas.draw_idle()

    def _clear_measurement(self, x, y):
        """Clears the current measurement and removes all markers."""
        self._clear_marker_a()
        self._clear_marker_b()

        self.endpoint_a = None
        self.endpoint_b = None

        self._restore_format_coord(x, y)

    def _restore_format_coord(self, x, y):
        """Restore the original format_coord method."""
        if self.original_format_coord is not None:
            try:
                # Check if axes still exists and has format_coord attribute
                if self.axes and hasattr(self.axes, 'format_coord'):
                    self.axes.format_coord = self.original_format_coord
            except Exception:
                # Ignore errors if axes or figure are already destroyed
                pass
        self.original_format_coord = None
        # Clear toolbar message if possible and redraw
        try:
            toolbar = self.fig.canvas.toolbar
            if toolbar is not None and hasattr(toolbar, 'set_message'):
                if x is not None and y is not None:
                    toolbar.set_message(self.axes.format_coord(x, y))
                else:
                    toolbar.set_message("")
        except Exception:
            pass  # Ignore if toolbar is gone

    def _display_delta(self, x, y):
        """
        Display the delta or single point values using the format_coord method.
        Updates the status bar message directly.
        """
        # Store the original format_coord method if not already stored
        if self.original_format_coord is None:
            try:
                if self.axes and hasattr(self.axes, 'format_coord'):
                    self.original_format_coord = self.axes.format_coord
                else:
                    return  # Cannot proceed if axes is invalid
            except Exception:
                return  # Cannot proceed if axes is invalid

        # Determine the message based on selected endpoints
        message = ""

        if self.endpoint_a is not None and self.endpoint_b is not None:
            xA, yA = self.endpoint_a
            xB, yB = self.endpoint_b
            deltaX = xB - xA
            deltaY = yB - yA

            if deltaX != 0 and deltaY == 0:
                message = f"X1: {xA:.6g}\nX2: {xB:.6g}\nΔX: {deltaX:.6g}\nY: {yA:.6g}"
            elif deltaX == 0 and deltaY != 0:
                message = f"X: {xA:.6g}\nY1: {yA:.6g}\nY2: {yB:.6g}\nΔY: {deltaY:.6g}"
            else:  # Covers both non-zero or both zero
                message = f"X1: {xA:.6g}, Y1: {yA:.6g}\nX2: {xB:.6g}, Y2: {yB:.6g}\nΔX: {deltaX:.6g}, ΔY: {deltaY:.6g}"

        elif self.endpoint_a is not None:
            xA, yA = self.endpoint_a
            message = f"X: {xA:.6g}\nY: {yA:.6g}"

        elif self.endpoint_b is not None:
            xB, yB = self.endpoint_b
            message = f"X: {xB:.6g}\nY: {yB:.6g}"
        else:
            # No points selected, restore original behavior
            self._restore_format_coord(x, y)

        # Create a custom format_coord function
        # Need to capture the original method in the closure
        original_format_coord_func = self.original_format_coord

        # Create a custom format_coord function that always shows the calculated message
        def custom_format_coord(x, y):
            # Get the original coordinate string
            try:
                coord_str = original_format_coord_func(x, y)
            except Exception:
                coord_str = f"(x, y) = ({x:.6g}, {y:.6g})"  # Fallback

            return f"{message}\n{coord_str}" if message else coord_str

        # Set the custom format_coord function
        try:
            if self.axes and hasattr(self.axes, 'format_coord'):
                self.axes.format_coord = custom_format_coord
            else:
                self._restore_format_coord(x, y)  # Restore if axes became invalid
        except Exception:
            self._restore_format_coord(x, y)  # Restore on error

        # Immediately update the navigation toolbar message with the calculated message
        try:
            toolbar = self.fig.canvas.toolbar
            if toolbar is not None and hasattr(toolbar, 'set_message'):
                # Use the pre-calculated message directly
                toolbar.set_message(custom_format_coord(x, y))
        except Exception:
            pass  # Ignore if figure/axes/toolbar are gone

    def _on_key_press(self, event):
        if event.inaxes != self.axes or event.key is None:
            return

        key = event.key.lower()  # Handle upper/lower case

        if key == 'a':
            nearest = self._find_nearest_point(event)
            if nearest is None:
                return  # Do nothing if no point is near

            if self.endpoint_a is None:
                self.endpoint_a = nearest
                self._draw_marker_a()
            elif nearest == self.endpoint_a:
                self._clear_marker_a()
                self.endpoint_a = None
            elif self.endpoint_b is not None and nearest == self.endpoint_b:
                self._clear_marker_b()
                self.endpoint_b = None
                self.endpoint_a = nearest
                self._draw_marker_a()
            else:
                self.endpoint_a = nearest
                self._draw_marker_a()

            self._display_delta(event.xdata, event.ydata)
        elif key == 's':
            segment = self._find_nearest_segment(event)

            if segment is not None:
                if segment[0] == self.endpoint_a and segment[1] == self.endpoint_b:
                    self._clear_measurement(event.xdata, event.ydata)
                else:
                    self.endpoint_a = segment[0]
                    self.endpoint_b = segment[1]
                    self._draw_marker_a()
                    self._draw_marker_b()
                    self._display_delta(event.xdata, event.ydata)

        elif key == 'd':
            nearest = self._find_nearest_point(event)
            if nearest is None:
                return  # Do nothing if no point is near

            if self.endpoint_b is None:
                # no -> D near point -> mark B -> B
                # A -> D near point -> mark B -> AB
                self.endpoint_b = nearest
                self._draw_marker_b()
            elif nearest == self.endpoint_b:
                # B -> D near B -> clear B -> no
                # AB -> D near B -> clear B -> A (state A handled implicitly by clearing B)
                self._clear_marker_b()
                self.endpoint_b = None
            elif self.endpoint_a is not None and nearest == self.endpoint_a:
                # AB -> D near A -> mark new B, unmark A -> B
                self._clear_marker_a()
                self.endpoint_a = None
                self.endpoint_b = nearest
                self._draw_marker_b()
            else:
                # B -> D near non-B -> mark new B -> B
                # AB -> D near non-A, non-B -> mark new B -> AB
                self.endpoint_b = nearest
                self._draw_marker_b()

            self._display_delta(event.xdata, event.ydata)

        elif key == 'x':

            # If a zoom or pan gesture is in progress, then the user probably
            # only wanted to affect that, and not clear the measurement:
            if getattr(self.fig.canvas.toolbar, "_pan_info", None) is None \
               and getattr(self.fig.canvas.toolbar, "_zoom_info", None) is None:

                if self.endpoint_a is not None or self.endpoint_b is not None:
                    # any -> X -> clear -> no
                    self._clear_measurement(event.xdata, event.ydata)

        return False

    def disconnect(self):
        """Disconnects the event handler."""
        if self.cid is not None:
            self.fig.canvas.mpl_disconnect(self.cid)
            self.cid = None
        self._restore_format_coord(None, None)  # Clean up format_coord on disconnect

    def __del__(self):
        # Ensure disconnection and cleanup when the object is deleted
        self.disconnect()
